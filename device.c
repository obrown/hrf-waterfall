#include "device.h"

#include <stdio.h>

#include "libhackrf/hackrf.h"


#define BLOCKS_PER_TRANSFER 16

#define SAMPLES_PER_BLOCK 8192

#define BYTES_PER_SAMPLE 2
#define BYTES_PER_BLOCK (SAMPLES_PER_BLOCK*BYTES_PER_SAMPLE)
#define BYTES_PER_TRANSFER (BYTES_PER_BLOCK*BLOCKS_PER_TRANSFER)


#define HRF_ASSERT(x, m) if (x) { fprintf(stderr, (m), hackrf_error_name(err)); }


static int recv_cb(hackrf_transfer *transfer);

static device_rx_cb cb = NULL;
static hackrf_device *device = NULL;
static hackrf_device_list_t *devices = NULL;


bool device_init(void) {
  int err;
  err = hackrf_init();
  HRF_ASSERT(err, "failed to init: %s\n");

  devices = hackrf_device_list();

  err = hackrf_device_list_open(devices, 0, &device);
  HRF_ASSERT(err, "failed to open device: %s\n");

  return !err;
}

bool device_destroy(void) {
  int err;
  err = hackrf_stop_rx(device);
  HRF_ASSERT(err, "failed to cleanly stop rx: %s\n");

  err = hackrf_close(device);
  HRF_ASSERT(err, "failed to cleanly close device: %s\n");

  hackrf_device_list_free(devices);

  err = hackrf_exit();
  HRF_ASSERT(err, "failed to exit cleanly: %s\n");

  return !err;
}

bool device_set_frequency(uint64_t frequency) {
  int err;
  err = hackrf_set_freq(device, frequency);
  HRF_ASSERT(err, "couldn't set frequency: %s\n");

  return !err;
}

bool device_set_sample_rate(uint64_t sample_rate) {
  int err;
  err = hackrf_set_sample_rate(device, sample_rate);
  HRF_ASSERT(err, "couldn't set sample rate: %s\n");

  return !err;
}

bool device_set_gain(uint32_t gain) {
  int err;
  err = hackrf_set_lna_gain(device, gain);
  HRF_ASSERT(err, "couldn't set lna gain: %s\n");

  return !err;
}

bool device_rx(device_rx_cb cb_fn) {
  int err;
  err = hackrf_start_rx(device, recv_cb, NULL);
  HRF_ASSERT(err, "couldn't start receiving: %s\n");

  cb = cb_fn;

  return !err;
}

bool device_is_alive() {
  return device && (hackrf_is_streaming(device) == HACKRF_TRUE);
}

static int recv_cb(hackrf_transfer *transfer) {
  cb(transfer->buffer, SAMPLES_PER_BLOCK * BLOCKS_PER_TRANSFER, BYTES_PER_SAMPLE);

  return 0;
}
