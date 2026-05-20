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
  COLOR_MODE  mode;         // Theme mode (dark or light).
  SHADE_MODE  cols16_mode;  // 16-color generation mode (darken or lighten).
  float       alpha;        // Alpha value for the palette.
  float       saturation;   // Saturation adjustment.
  float       contrast;     // Contrast adjustment.
  char       *backend;      // Image processing backend name.
  char       *script_path;  // Post-hook script path.
  char       *out_dir;      // Output directory for generated files.
  char       *random_dir;   // Directory for random image selection.
  bool        skip_cursor;  // If true, skip writing the OSC 12 cursor color sequence.
} AppOptions;

typedef struct {
  AppOptions  opts;        // All shared persistent+CLI-overridable options.
  Link       *links;       // Array of file links (config-only).
  int         num_links;   // Current number of links (config-only).
} Config;

Config *load_config(void);
void save_config(const Config *config);
void free_config(Config *config);
