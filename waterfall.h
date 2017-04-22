#ifndef __WATERFALL_H__
#define __WATERFALL_H__

#include "SDL2/SDL.h"

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  uint8_t *pixels;
  size_t width;
  size_t height;
  float min_intensity;
  float max_intensity;
} waterfall_t;


void waterfall_init(waterfall_t *w, size_t width, size_t height);

void waterfall_destroy(waterfall_t *w);

void waterfall_newline(waterfall_t *w, float *line, size_t line_len);


#endif // __WATERFALL_H__
