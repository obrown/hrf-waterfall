#include "waterfall.h"

#include "colormap.h"

#include <stdlib.h>


#define ALPHA 0.05f


static void exp_window_update(waterfall_t *w, float *line, size_t line_len);
uint32_t colour_intensity(uint8_t intensity);

void waterfall_init(waterfall_t *w, size_t width, size_t height) {
  w->pixels = calloc(width * height * 4, sizeof(uint8_t));

  SDL_Init(SDL_INIT_VIDEO);
  w->window = SDL_CreateWindow("", 100, 100, width, height, 0);
  w->renderer = SDL_CreateRenderer(w->window, -1, SDL_RENDERER_ACCELERATED);
  w->texture = SDL_CreateTexture(w->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

  w->width = width;
  w->height = height;
  w->max_intensity = 0;
  w->min_intensity = -128;

  SDL_RenderClear(w->renderer);
  SDL_RenderPresent(w->renderer);
}

void waterfall_destroy(waterfall_t *w) {
  SDL_DestroyTexture(w->texture);
  SDL_DestroyRenderer(w->renderer);
  SDL_DestroyWindow(w->window);
  SDL_Quit();
  free(w->pixels);
}

void waterfall_newline(waterfall_t *w, float *line, size_t line_len) {
  exp_window_update(w, line, line_len);

  memmove(w->pixels + w->width * 4, w->pixels, (w->height - 1) * w->width * 4);

  unsigned int points_per_pixel = line_len / w->width;
  for (unsigned int i = 0; i < w->width; i++) {
    float sum = 0;
    for (unsigned int j = 0; j < points_per_pixel; j++)
      sum += line[i*points_per_pixel + j];

    sum /= points_per_pixel;
    uint8_t gray = interpolate(sum, 0.0, w->min_intensity, 1.0, w->max_intensity) * 255;
    ((uint32_t *)w->pixels)[i] = colour_intensity(gray);
  }


  SDL_UpdateTexture(w->texture, NULL, w->pixels, w->width * 4);
  SDL_RenderCopy(w->renderer, w->texture, NULL, NULL);
  SDL_RenderPresent(w->renderer);
}

uint32_t colour_intensity(uint8_t intensity) {
  return 255 << 24 | intensity << 16 | intensity << 8 | intensity;
}

static void exp_window_update(waterfall_t *w, float *line, size_t line_len) {
  float min_mag = line[8];
  float max_mag = line[8];

  for (unsigned int x = 0; x < line_len; x++)
    if (line[x] < min_mag) min_mag = line[x];

  for (unsigned int x = 0; x < line_len; x++)
    if (line[x] > max_mag) max_mag = line[x];

  w->min_intensity = ALPHA * min_mag + (1 - ALPHA) * w->min_intensity;
  w->max_intensity = ALPHA * max_mag + (1 - ALPHA) * w->max_intensity;
}
