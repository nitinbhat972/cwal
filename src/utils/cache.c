#include "cache.h"
#include "path.h"
#include "utils.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

static void generate_cache_filename(char *buffer, size_t buffer_size,
                                    const Palette *palette,
                                    const char *cache_dir,
                                    const char *backend_name) {
  char *expanded_cache_dir = expand_home(cache_dir);
  char *filename = strrchr(palette->wallpaper, '/');
  if (filename) {
    filename++;
  } else {
    filename = (char *)palette->wallpaper;
  }

  const char *mode_str = (palette->mode == DARK) ? "dark" : "light";
  const char *cols16_mode_str =
      (palette->cols16_mode == DARKEN) ? "darken" : "lighten";

  snprintf(buffer, buffer_size, "%s/schemes/%s_%s_%s_s%.2f_c%.2f_a%.2f_%s.cwal",
           expanded_cache_dir, filename, mode_str, cols16_mode_str,
           palette->saturation, palette->contrast, palette->alpha,
           backend_name);
  free(expanded_cache_dir);
}

void save_palette_to_cache(const Palette *palette, const char *cache_dir,
                           const char *backend_name) {
  char cache_filepath[PATH_MAX];
  generate_cache_filename(cache_filepath, sizeof(cache_filepath), palette,
                          cache_dir, backend_name);

  char *cache_dir_path = build_path(expand_home(cache_dir), "schemes");

  if (validate_or_create_dir(cache_dir_path) != 0) {
    logging(ERROR, "Failed to create cache directory: %s", cache_dir_path);
    free(cache_dir_path);
    return;
  }

  FILE *file = fopen(cache_filepath, "w");
  if (!file) {
    logging(ERROR, "Failed to open cache file for writing: %s", cache_filepath);
    free(cache_dir_path);
    return;
  }
  free(cache_dir_path);

  fprintf(file, "wallpaper=%s\n", palette->wallpaper);

  for (int i = 0; i < PALETTE_MAX_SIZE; i++) {
    fprintf(file, "color%d=%d,%d,%d\n", i, palette->colors[i].red,
            palette->colors[i].green, palette->colors[i].blue);
  }

  fclose(file);
  logging(INFO, "Palette saved to cache: %s", cache_filepath);
}

int load_palette_from_cache(Palette *palette, const char *cache_dir,
                            const char *backend_name) {
  char cache_filepath[PATH_MAX];
  generate_cache_filename(cache_filepath, sizeof(cache_filepath), palette,
                          cache_dir, backend_name);

  FILE *file = fopen(cache_filepath, "r");
  if (!file) {
    return -1;
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file)) {
    char *key = strtok(line, "=");
    char *value = strtok(NULL, "\n");

    if (!key || !value)
      continue;

    if (strcmp(key, "wallpaper") == 0) {
      if (strcmp(value, palette->wallpaper) != 0) {
        fclose(file);
        return -1;
      }
    } else if (strcmp(key, "alpha") == 0) {
    } else if (strncmp(key, "color", 5) == 0) {
      int index = atoi(key + 5);
      if (index >= 0 && index < PALETTE_MAX_SIZE) {
        int r, g, b;
        if (sscanf(value, "%d,%d,%d", &r, &g, &b) == 3) {
          palette->colors[index] = (Color){(uint8_t)r, (uint8_t)g, (uint8_t)b};
        } else {
          logging(WARN, "Failed to parse color from cache file for index %d.",
                  index);
        }
      }
    }
  }

  fclose(file);
  logging(INFO, "Found cache: %s", cache_filepath);
  return 0;
}
