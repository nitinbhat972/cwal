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

#include "core.h"

#define MAX_LINE_LENGTH 256

typedef enum {
  RANDOM_DARK,
  RANDOM_LIGHT,
  RANDOM_ALL,
} RandomMode;

int load_theme(Palette *palette, const char *theme_name);
int load_random_theme(Palette *palette, RandomMode mode);
void list_themes();
