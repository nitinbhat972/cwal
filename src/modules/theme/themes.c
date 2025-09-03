#include "themes.h"
#include "utils/path.h"
#include "utils/utils.h"
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static char **get_theme_dirs() {
  char *user_config_themes = expand_home("~/.config/cwal/themes");
  char *user_local_themes = expand_home("~/.local/share/cwal/themes");

  const char *theme_dirs[] = {user_config_themes, user_local_themes,
                              "/usr/local/share/cwal/themes", NULL};

  char **dirs = malloc(sizeof(char *) * 4);
  for (int i = 0; theme_dirs[i] != NULL; i++) {
    dirs[i] = strdup(theme_dirs[i]);
  }
  dirs[3] = NULL;

  free(user_config_themes);
  free(user_local_themes);

  return dirs;
}

int load_theme(Palette *palette, const char *theme_name) {
  char **theme_dirs = get_theme_dirs();
  char theme_path[PATH_MAX];
  FILE *file = NULL;

  for (int i = 0; theme_dirs[i] != NULL; i++) {
    if (!theme_dirs[i])
      continue;

    struct stat st;
    if (stat(theme_dirs[i], &st) != 0 || !S_ISDIR(st.st_mode)) {
      continue;
    }

    char *dark_path = build_path(theme_dirs[i], "dark");
    char *full_path = build_path(dark_path, theme_name);
    snprintf(theme_path, sizeof(theme_path), "%s.cwal", full_path);
    file = fopen(theme_path, "r");
    free(dark_path);
    free(full_path);
    if (file)
      break;

    char *light_path = build_path(theme_dirs[i], "light");
    full_path = build_path(light_path, theme_name);
    snprintf(theme_path, sizeof(theme_path), "%s.cwal", full_path);
    file = fopen(theme_path, "r");
    free(light_path);
    free(full_path);
    if (file)
      break;
  }

  for (int i = 0; theme_dirs[i] != NULL; i++) {
    free(theme_dirs[i]);
  }
  free(theme_dirs);

  if (!file) {
    logging(ERROR, "Theme not found in any theme directory: %s", theme_name);
    return -1;
  }

  char line[MAX_LINE_LENGTH];
  char *saveptr;
  while (fgets(line, sizeof(line), file)) {
    char *key = strtok_r(line, "=", &saveptr);
    char *value = strtok_r(NULL, "\n", &saveptr);

    if (!key || !value)
      continue;

    if (strncmp(key, "mode", 5) == 0) {
      if (strncmp(value, "dark", 5) == 0) {
        palette->mode = DARK;
      } else if (strncmp(value, "light", 6) == 0) {
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
  char **theme_dirs = get_theme_dirs();
  char **themes = NULL;
  int count = 0;

  for (int i = 0; theme_dirs[i] != NULL; i++) {
    if (!theme_dirs[i])
      continue;

    struct stat st;
    if (stat(theme_dirs[i], &st) != 0 || !S_ISDIR(st.st_mode)) {
      continue;
    }

    if (mode == RANDOM_DARK || mode == RANDOM_ALL) {
      char *dark_path = build_path(theme_dirs[i], "dark");
      DIR *d = opendir(dark_path);
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
      free(dark_path);
    }

    if (mode == RANDOM_LIGHT || mode == RANDOM_ALL) {
      char *light_path = build_path(theme_dirs[i], "light");
      DIR *d = opendir(light_path);
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
      free(light_path);
    }
  }

  for (int i = 0; theme_dirs[i] != NULL; i++) {
    free(theme_dirs[i]);
  }
  free(theme_dirs);

  if (count == 0) {
    logging(ERROR, "No themes found in the specified directories.");
    return -1;
  }

  unsigned int seed = time(NULL);
  int random_index = rand_r(&seed) % count;
  char *theme_name = themes[random_index];

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
  char **theme_dirs = get_theme_dirs();

  for (int i = 0; theme_dirs[i] != NULL; i++) {
    if (!theme_dirs[i])
      continue;

    struct stat st;
    if (stat(theme_dirs[i], &st) != 0 || !S_ISDIR(st.st_mode)) {
      continue;
    }

    printf("Themes in %s:\n", theme_dirs[i]);
    char *dark_path = build_path(theme_dirs[i], "dark");
    DIR *d_dark = opendir(dark_path);
    if (d_dark) {
      printf("  Dark themes:\n");
      struct dirent *dir;
      while ((dir = readdir(d_dark)) != NULL) {
        if (dir->d_type == DT_REG) {
          printf("    %s\n", dir->d_name);
        }
      }
      closedir(d_dark);
    }
    free(dark_path);

    char *light_path = build_path(theme_dirs[i], "light");
    DIR *d_light = opendir(light_path);
    if (d_light) {
      printf("  Light themes:\n");
      struct dirent *dir;
      while ((dir = readdir(d_light)) != NULL) {
        if (dir->d_type == DT_REG) {
          printf("    %s\n", dir->d_name);
        }
      }
      closedir(d_light);
    }
    free(light_path);
    printf("\n");
  }

  for (int i = 0; theme_dirs[i] != NULL; i++) {
    free(theme_dirs[i]);
  }
  free(theme_dirs);
}
