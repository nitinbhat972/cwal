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

typedef struct {
  char *template_name;
  char *target_path;
  char *reload_cmd;
} Link;

typedef struct {
  char *backend;           // Name of the image processing backend to use.
  COLOR_MODE mode;         // Theme mode (dark or light).
  SHADE_MODE cols16_mode;  // 16-color generation mode (darken or lighten).
  float alpha;             // Alpha value for the palette.
  float saturation;        // Saturation adjustment.
  float contrast;          // Contrast adjustment.
  char *script_path;       // Path to a script to run after processing.
  char *random_dir;        // Path to a directory for random image selection.
  char *out_dir;           // Output directory for generated files.
  Link *links;             // Array of file links.
  int num_links;           // Current number of links.
} Config;

Config *load_config(void);
void save_config(const Config *config);
void free_config(Config *config);
