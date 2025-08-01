#include "themes.h"
#include "utils.h"
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define THEME_DIR "/usr/local/share/cwal/themes/"
#define MAX_LINE_LENGTH 256

int load_theme(Palette *palette, const char *theme_name) {
  char theme_path[PATH_MAX];
  snprintf(theme_path, sizeof(theme_path), "%s/dark/%s.cwal", THEME_DIR,
           theme_name);

  FILE *file = fopen(theme_path, "r");
  if (!file) {
    snprintf(theme_path, sizeof(theme_path), "%s/light/%s.cwal", THEME_DIR,
             theme_name);
    file = fopen(theme_path, "r");
    if (!file) {
      logging(ERROR, "Theme not found in dark or light theme directory: %s",
              theme_name);
      return -1;
    }
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file)) {
    char *key = strtok(line, "=");
    char *value = strtok(NULL, "\n");

    if (!key || !value)
      continue;

    if (strcmp(key, "mode") == 0) {
      if (strcmp(value, "dark") == 0) {
        palette->mode = DARK;
      } else if (strcmp(value, "light") == 0) {
        palette->mode = LIGHT;
      }
    } else if (strncmp(key, "color", 5) == 0) {
      int index = atoi(key + 5);
      if (index >= 0 && index < PALETTE_MAX_SIZE) {
        int r, g, b;
        if (sscanf(value, "%d,%d,%d", &r, &g, &b) == 3) {
          palette->colors[index] = (Color){(uint8_t)r, (uint8_t)g, (uint8_t)b};
        }
      }
    }
  }

  fclose(file);
  palette->wallpaper = NULL;
  logging(INFO, "Loaded theme: %s", theme_name);
  return 0;
}

int load_random_theme(Palette *palette, RandomMode mode) {
  const char *dark_themes_path = THEME_DIR "dark/";
  const char *light_themes_path = THEME_DIR "light/";

  char **themes = NULL;
  int count = 0;

  if (mode == RANDOM_DARK || mode == RANDOM_ALL) {
    DIR *d = opendir(dark_themes_path);
    if (d) {
      struct dirent *dir;
      while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
          themes = realloc(themes, sizeof(char *) * (count + 1));
          themes[count++] = strdup(dir->d_name);
        }
      }
      closedir(d);
    }
  }

  if (mode == RANDOM_LIGHT || mode == RANDOM_ALL) {
    DIR *d = opendir(light_themes_path);
    if (d) {
      struct dirent *dir;
      while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
          themes = realloc(themes, sizeof(char *) * (count + 1));
          themes[count++] = strdup(dir->d_name);
        }
      }
      closedir(d);
    }
  }

  if (count == 0) {
    logging(ERROR, "No themes found in the specified directories.");
    return -1;
  }

  unsigned int seed = time(NULL);
  int random_index = rand_r(&seed) % count;
  char *theme_name = themes[random_index];

  // remove .cwal from theme name
  char *dot = strrchr(theme_name, '.');
  if (dot) {
    *dot = '\0';
  }

  int result = load_theme(palette, theme_name);

  for (int i = 0; i < count; i++) {
    free(themes[i]);
  }
  free(themes);

  return result;
}

void list_themes() {
  const char *dark_themes_path = THEME_DIR "dark/";
  const char *light_themes_path = THEME_DIR "light/";

  printf("Dark themes:\n");
  DIR *d_dark = opendir(dark_themes_path);
  if (d_dark) {
    struct dirent *dir;
    while ((dir = readdir(d_dark)) != NULL) {
      if (dir->d_type == DT_REG) {
        printf("  %s\n", dir->d_name);
      }
    }
    closedir(d_dark);
  }

  printf("\nLight themes:\n");
  DIR *d_light = opendir(light_themes_path);
  if (d_light) {
    struct dirent *dir;
    while ((dir = readdir(d_light)) != NULL) {
      if (dir->d_type == DT_REG) {
        printf("  %s\n", dir->d_name);
      }
    }
    closedir(d_light);
  }
}
