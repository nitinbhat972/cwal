#pragma once
#include <stdbool.h>
#include <stdint.h>

#define PALETTE_MAX_SIZE 16

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} Color;

typedef enum { DARK, LIGHT } COLOR_MODE;

typedef enum { LIGHTEN, DARKEN } SHADE_MODE;

typedef struct {
  const char *wallpaper;
  Color colors[PALETTE_MAX_SIZE];
  float saturation;
  float contrast;
  float alpha;
  SHADE_MODE cols16_mode;
  COLOR_MODE mode;
} Palette;
