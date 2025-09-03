#pragma once

#include "core.h"

#define CONFIG_PATH "~/.config/cwal/cwal.ini"
#define CONFIG_DIR "~/.config/cwal/"
#define MAX_LINE_LENGTH 256

typedef struct {
  char *current_wallpaper; // Path to the currently set wallpaper.
  char *backend;           // Name of the image processing backend to use.
  COLOR_MODE mode;         // Theme mode (dark or light).
  SHADE_MODE cols16_mode;  // 16-color generation mode (darken or lighten).
  float alpha;             // Alpha value for the palette.
  char *out_dir;           // Output directory for generated files.
} Config;

Config *load_config(void);
void save_config(const Config *config);
void free_config(Config *config);
