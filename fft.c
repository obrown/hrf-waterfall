#include "fft.h"

#include "fftw3.h"

#include <math.h>
#include <stdlib.h>


#define PI 3.14159265358979


static float *fft_window;
static fftw_plan fft_plan;
static size_t fft_size;


static float logPower(fftw_complex c, float scale) {
  float re = c[0] * scale;
  float im = c[1] * scale;
  float magsq = re * re + im * im;
  return log2f(magsq) * 10.0f / log2(10.0f);
}

float *fft(int8_t *samples) {
  fftw_complex *fft_in  = fftw_malloc(sizeof(fftw_complex) * fft_size);
  fftw_complex *fft_out = fftw_malloc(sizeof(fftw_complex) * fft_size);

  #pragma omp simd
  for (size_t i = 0; i < fft_size; i++) {
    fft_in[i][0] = samples[2*i] * fft_window[i] * (1.0f / 128.0f);
    fft_in[i][1] = samples[2*i + 1] * fft_window[i] * (1.0f / 128.0f);
  }

  fftw_execute_dft(fft_plan, fft_in, fft_out);

  float *line = malloc(sizeof(float) * fft_size);
  #pragma omp simd
  for (size_t i = 0; i < fft_size; i++) {
    line[i] = logPower(fft_out[i], 1.0f / fft_size);
  }

  line[0] = line[1]; // remove dc bias

  fftw_free(fft_out);
  fftw_free(fft_in);

  return line;
}

void fft_init(size_t size) {
  fft_size = size;

  fft_window = malloc(sizeof(float) * fft_size);
  for (size_t i = 0; i < fft_size; i++) {
    fft_window[i] = 0.5f * (1.0f - cos(2 * PI * i / (fft_size - 1)));
  }

  fftw_complex *fft_in  = fftw_malloc(sizeof(fftw_complex) * fft_size);
  fftw_complex *fft_out = fftw_malloc(sizeof(fftw_complex) * fft_size);
  fft_plan = fftw_plan_dft_1d(fft_size, fft_in, fft_out, FFTW_FORWARD, FFTW_MEASURE | FFTW_DESTROY_INPUT);
  fftw_free(fft_out);
  fftw_free(fft_in);
}

void fft_destroy() {
  free(fft_window);
  fftw_destroy_plan(fft_plan);
  fftw_cleanup();
}
