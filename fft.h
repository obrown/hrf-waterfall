#ifndef __FFT_H__
#define __FFT_H__

#include <stddef.h>
#include <stdint.h>

float *fft(int8_t *samples);
void fft_destroy();
void fft_init(size_t size);

#endif //__FFT_H__
