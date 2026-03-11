/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "config.h"
#include "utils/path.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void parse_key_value(Config *config, const char *key,
                            const char *value) {
  if (strncmp(key, "out_dir", 8) == 0) {
    // If value is empty, do not update, keep default
    if (strlen(value) > 0) {
      free(config->out_dir);
      config->out_dir = expand_home(value);
      if (!config->out_dir) {
        logging(ERROR, "Failed to allocate out_dir");
        return;
      }
    }
  } else if (strncmp(key, "current_wallpaper", 18) == 0) {
    free(config->current_wallpaper);
    config->current_wallpaper = expand_home(value);
    if (!config->current_wallpaper) {
      logging(ERROR, "Failed to allocate current_wallpaper");
      return;
    }
  } else if (strncmp(key, "backend", 8) == 0) {
    free(config->backend);
    config->backend = strdup(value);
    if (!config->backend) {
      logging(ERROR, "Failed to allocate backend");
      return;
    }
  } else if (strncmp(key, "alpha", 6) == 0) {
    if (strlen(value) > 0) {
      config->alpha = atof(value);
    }
  } else if (strncmp(key, "saturation", 11) == 0) {
    if (strlen(value) > 0) {
      config->saturation = atof(value);
    }
  } else if (strncmp(key, "contrast", 9) == 0) {
    if (strlen(value) > 0) {
      config->contrast = atof(value);
    }
  } else if (strncmp(key, "script_path", 12) == 0) {
    if (strlen(value) > 0) {
      free(config->script_path);
      config->script_path = expand_home(value);
    }
  } else if (strncmp(key, "random_dir", 11) == 0) {
    if (strlen(value) > 0) {
      free(config->random_dir);
      config->random_dir = expand_home(value);
    }
  } else if (strncmp(key, "mode", 5) == 0) {
    if (strlen(value) > 0) {
      if (strncmp(value, "dark", 5) == 0) {
        config->mode = DARK;
      } else if (strncmp(value, "light", 6) == 0) {
        config->mode = LIGHT;
      } else {
        logging(WARN, "Invalid mode value in config: %s. Using default.",
                value);
      }
    }
  } else if (strncmp(key, "cols16_mode", 12) == 0) {
    if (strlen(value) > 0) {
      if (strncmp(value, "darken", 7) == 0) {
        config->cols16_mode = DARKEN;
      } else if (strncmp(value, "lighten", 8) == 0) {
        config->cols16_mode = LIGHTEN;
      } else if (strncmp(value, "none", 5) == 0) {
        config->cols16_mode = NONE;
      } else {
        logging(WARN, "Invalid cols16_mode value in config: %s. Using default.",
                value);
      }
    }
  }
}

static void create_config_subdirectories(const char *config_dir_path) {
  char *templates_dir = build_path(config_dir_path, "templates");
  if (templates_dir) {
    if (validate_or_create_dir(templates_dir) != 0) {
      logging(ERROR, "Failed to create templates directory: %s", templates_dir);
    }
    free(templates_dir);
  }

  char *themes_dir = build_path(config_dir_path, "themes");
  if (themes_dir) {
    if (validate_or_create_dir(themes_dir) != 0) {
      logging(ERROR, "Failed to create themes directory: %s", themes_dir);
    }

    char *dark_themes_dir = build_path(themes_dir, "dark");
    if (dark_themes_dir) {
      if (validate_or_create_dir(dark_themes_dir) != 0) {
        logging(ERROR, "Failed to create dark themes directory: %s",
                dark_themes_dir);
      }
      free(dark_themes_dir);
    }

    char *light_themes_dir = build_path(themes_dir, "light");
    if (light_themes_dir) {
      if (validate_or_create_dir(light_themes_dir) != 0) {
        logging(ERROR, "Failed to create light themes directory: %s",
                light_themes_dir);
      }
      free(light_themes_dir);
    }
    free(themes_dir);
  }

  char *backends_dir = build_path(config_dir_path, "backends");
  if (backends_dir) {
    if (validate_or_create_dir(backends_dir) != 0) {
      logging(ERROR, "Failed to create backends directory: %s", backends_dir);
    }
    free(backends_dir);
  }
}

Config *load_config(void) {
  Config *config = calloc(1, sizeof(Config));
  if (!config) {
    perror("Failed to allocate config");
    return NULL;
  }

  // Set default values (these can be overridden by CLI arguments)
  config->out_dir = expand_home("~/.cache/cwal/"); // Default out_dir
  config->current_wallpaper = NULL;
  config->backend = NULL;
  config->mode = DARK;
  config->cols16_mode = DARKEN;
  config->alpha = 1.0;
  config->saturation = 0.0;
  config->contrast = 1.0;
  config->script_path = NULL;
  config->random_dir = NULL;

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

    if (len == 0 || trimmed_line[0] == '#' || trimmed_line[0] == ';') {
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

        parse_key_value(config, key, value);
      }
    }
  }

  fclose(file);
  free(expanded_path);
  return config;
}

void save_config(const Config *config) {
  char *expanded_path = expand_home(CONFIG_PATH);
  char *config_dir_path = expand_home(CONFIG_DIR);
  if (validate_or_create_dir(config_dir_path) != 0) {
    logging(ERROR, "Failed to create config directory: %s", config_dir_path);
    free(config_dir_path);
    free(expanded_path);
    return;
  }
  create_config_subdirectories(config_dir_path);
  free(config_dir_path);

  FILE *file = fopen(expanded_path, "w");
  if (!file) {
    logging(ERROR, "Failed to open config file for writing: %s", expanded_path);
    free(expanded_path);
    return;
  }

  fprintf(file, "[general]\n");
  fprintf(file, "out_dir = %s\n", config->out_dir);
  fprintf(file, "current_wallpaper = %s\n", config->current_wallpaper ? config->current_wallpaper : "");
  fprintf(file, "backend = %s\n", config->backend ? config->backend : "cwal");
  fprintf(file, "script_path = %s\n", config->script_path ? config->script_path : "");

  fprintf(file, "\n[options]\n");
  fprintf(file, "alpha = %.2f\n", config->alpha);
  fprintf(file, "saturation = %.2f\n", config->saturation);
  fprintf(file, "contrast = %.2f\n", config->contrast);
  fprintf(file, "mode = %s\n", config->mode == DARK ? "dark" : "light");
  fprintf(file, "cols16_mode = %s\n",
          config->cols16_mode == DARKEN
              ? "darken"
              : (config->cols16_mode == LIGHTEN ? "lighten" : "none"));

  fprintf(file, "\n[random]\n");
  fprintf(file, "random_dir = %s\n", config->random_dir ? config->random_dir : "");

  fclose(file);
  free(expanded_path);
  logging(INFO, "Config saved to %s", CONFIG_PATH);
}

void free_config(Config *config) {
  if (config) {
    free(config->out_dir);
    free(config->current_wallpaper);
    free(config->backend);
    free(config->script_path);
    free(config->random_dir);
    free(config);
  }
}
