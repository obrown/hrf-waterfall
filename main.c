#include "queue.h"
#include "waterfall.h"
#include "device.h"
#include "fft.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <signal.h>


#define DISPLAY_WIDTH 2048
#define DISPLAY_HEIGHT 1024

#define SAMPLE_RATE 1000000
#define FREQUENCY 153350000
#define LNA_GAIN 32

#define FFT_SIZE 8192  // max BYTES_PER_TRANSFER


static bool exit_requested = false;
static queue_t mag_line_queue;
static waterfall_t waterfall;


void recv_cb(void *samples, size_t n_samples, size_t bytes_per_sample) {
  int8_t *buff = (int8_t*)samples;
  size_t ffts = n_samples / FFT_SIZE;

  // TODO store in a buffer for fft size larger than n samples

  for (size_t i = 0; i < ffts; i++) {
    float *line = fft(buff + i * FFT_SIZE * bytes_per_sample);
    queue_append(&mag_line_queue, line);
  }
}

void sig_handler(int signo) {
  if (signo == SIGINT) {
    exit_requested = true;
  }
}

int main(void) {
  signal(SIGINT, sig_handler);

  queue_init(&mag_line_queue);
  waterfall_init(&waterfall, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  fft_init(FFT_SIZE);

  device_init();
  device_set_sample_rate(SAMPLE_RATE);
  device_set_frequency(FREQUENCY);
  device_set_gain(LNA_GAIN);

  device_rx(recv_cb);

  while (!exit_requested && device_is_alive()) {
    float *line = queue_pop(&mag_line_queue);
    if (line) {
      waterfall_newline(&waterfall, line, FFT_SIZE);
      free(line);
    }
  }

  fft_destroy();
  device_destroy();
  waterfall_destroy(&waterfall);
  queue_destroy(&mag_line_queue);
  return EXIT_SUCCESS;
}
