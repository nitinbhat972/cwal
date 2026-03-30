/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

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

typedef enum { NONE, LIGHTEN, DARKEN } SHADE_MODE;

typedef struct {
  char *wallpaper;
  Color colors[PALETTE_MAX_SIZE];
  float saturation;
  float contrast;
  float alpha;
  SHADE_MODE cols16_mode;
  COLOR_MODE mode;
} Palette;
