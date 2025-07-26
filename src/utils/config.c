#include "config.h"
#include "path.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *resolve_and_strdup_path(const char *path) {
  return expand_home(path);
}

static void parse_key_value(Config *config, const char *key,
                            const char *value) {
  if (strcmp(key, "out_dir") == 0) {
    // If value is empty, do not update, keep default
    if (strlen(value) > 0) {
      free(config->out_dir);
      config->out_dir = resolve_and_strdup_path(value);
      if (!config->out_dir) {
        logging(ERROR, "Failed to allocate out_dir");
        return;
      }
    }
  } else if (strcmp(key, "current_wallpaper") == 0) {
    free(config->current_wallpaper);
    config->current_wallpaper = resolve_and_strdup_path(value);
    if (!config->current_wallpaper) {
      logging(ERROR, "Failed to allocate current_wallpaper");
      return;
    }
  } else if (strcmp(key, "backend") == 0) {
    free(config->backend);
    config->backend = strdup(value);
    if (!config->backend) {
      logging(ERROR, "Failed to allocate backend");
      return;
    }
  } else if (strcmp(key, "mode") == 0) {
    if (strlen(value) > 0) {
      if (strcmp(value, "dark") == 0) {
        config->mode = DARK;
      } else if (strcmp(value, "light") == 0) {
        config->mode = LIGHT;
      } else {
        logging(WARN, "Invalid mode value in config: %s. Using default.",
                value);
      }
    }
  } else if (strcmp(key, "cols16_mode") == 0) {
    if (strlen(value) > 0) {
      if (strcmp(value, "darken") == 0) {
        config->cols16_mode = DARKEN;
      } else if (strcmp(value, "lighten") == 0) {
        config->cols16_mode = LIGHTEN;
      } else {
        logging(WARN, "Invalid cols16_mode value in config: %s. Using default.",
                value);
      }
    }
  }
}

Config *load_config(void) {
  Config *config = calloc(1, sizeof(Config));
  if (!config) {
    perror("Failed to allocate config");
    return NULL;
  }

  // Set default values (these can be overridden by CLI arguments)
  config->out_dir = resolve_and_strdup_path("~/.cache/cwal/"); // Default out_dir
  config->current_wallpaper = NULL; // Not loaded from config, set by CLI
  config->backend = NULL; // Not loaded from config, set by CLI
  config->mode = DARK; // Default mode
  config->cols16_mode = DARKEN; // Default 16-color generation mode

  char *expanded_path = expand_home(CONFIG_PATH);
  FILE *file = fopen(expanded_path, "r");
  if (!file) {
    logging(WARN, "Config file not found, using default values.");
    free(expanded_path);
    return config;
  }

  char line[MAX_LINE_LENGTH];
  char section[64] = "";

  while (fgets(line, sizeof(line), file)) {
    char *trimmed_line = line;
    while (*trimmed_line == ' ' || *trimmed_line == '\t')
      trimmed_line++;
    size_t len = strlen(trimmed_line);
    while (len > 0 &&
           (trimmed_line[len - 1] == '\n' || trimmed_line[len - 1] == '\r' ||
            trimmed_line[len - 1] == ' ' || trimmed_line[len - 1] == '\t')) {
      trimmed_line[--len] = '\0';
    }

    if (len == 0 || trimmed_line[0] == '#') {
      continue;
    }

    if (trimmed_line[0] == '[' && trimmed_line[len - 1] == ']') {
      strncpy(section, trimmed_line + 1, len - 2);
      section[len - 2] = '\0';
    } else if (strlen(section) > 0) {
      char *equals = strchr(trimmed_line, '=');
      if (equals) {
        *equals = '\0';
        char *key = trimmed_line;
        char *value = equals + 1;
        while (*key == ' ' || *key == '\t')
          key++;
        len = strlen(key);
        while (len > 0 && (key[len - 1] == ' ' || key[len - 1] == '\t'))
          key[--len] = '\0';
        while (*value == ' ' || *value == '\t')
          value++;

        if (strcmp(key, "out_dir") == 0) {
          if (strlen(value) > 0) {
            free(config->out_dir);
            config->out_dir = resolve_and_strdup_path(value);
            if (!config->out_dir) {
              logging(ERROR, "Failed to allocate out_dir");
              return NULL;
            }
          }
        }
      }
    }
  }

  fclose(file);
  free(expanded_path);
  return config;
}

void save_config(const Config *config) {
  char *expanded_path = expand_home(CONFIG_PATH);
  FILE *file = fopen(expanded_path, "w");
  if (!file) {
    logging(ERROR, "Failed to open config file for writing: %s", expanded_path);
    free(expanded_path);
    return;
  }

  fprintf(file, "[general]\n");
  fprintf(file, "out_dir = %s\n", config->out_dir);
  fprintf(file, "current_wallpaper = %s\n", config->current_wallpaper);
  fprintf(file, "backend = %s\n", config->backend);
  fprintf(file, "\n[theme]\n");
  fprintf(file, "mode = %s\n", config->mode == DARK ? "dark" : "light");
  fprintf(file, "cols16_mode = %s\n",
          config->cols16_mode == DARKEN ? "darken" : "lighten");

  fclose(file);
  free(expanded_path);
  logging(INFO, "Config saved to %s", CONFIG_PATH);
}

void free_config(Config *config) {
  if (config) {
    free(config->out_dir);
    free(config->current_wallpaper);
    free(config->backend);
    free(config);
  }
}
