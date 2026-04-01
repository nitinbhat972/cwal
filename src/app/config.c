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
    if (strlen(value) > 0) {
      char *new_value = strdup(value);
      if (!new_value) {
        logging(ERROR, "Failed to allocate out_dir");
        return;
      }
      free(config->out_dir);
      config->out_dir = new_value;
    }
  } else if (strncmp(key, "backend", 8) == 0) {
    char *new_value = strdup(value);
    if (!new_value) {
      logging(ERROR, "Failed to allocate backend");
      return;
    }
    free(config->backend);
    config->backend = new_value;
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
      char *new_value = strdup(value);
      if (!new_value) {
        logging(ERROR, "Failed to allocate script_path");
        return;
      }
      free(config->script_path);
      config->script_path = new_value;
    }
  } else if (strncmp(key, "random_dir", 11) == 0) {
    if (strlen(value) > 0) {
      char *new_value = strdup(value);
      if (!new_value) {
        logging(ERROR, "Failed to allocate random_dir");
        return;
      }
      free(config->random_dir);
      config->random_dir = new_value;
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

static void parse_link(Config *config, const char *key, const char *value) {
  // key = template_name
  // value = target_path | reload_cmd
  
  if (!key || !value || strlen(key) == 0 || strlen(value) == 0) return;

  char *val_copy = strdup(value);
  if (!val_copy) return;

  char *pipe = strchr(val_copy, '|');
  char *target_raw = val_copy;
  char *cmd_raw = NULL;

  if (pipe) {
    *pipe = '\0';
    cmd_raw = pipe + 1;
  }

  // Trim target_path
  while (*target_raw == ' ' || *target_raw == '\t') target_raw++;
  size_t t_len = strlen(target_raw);
  while (t_len > 0 && (target_raw[t_len-1] == ' ' || target_raw[t_len-1] == '\t')) {
    target_raw[--t_len] = '\0';
  }

  // Trim reload_cmd
  if (cmd_raw) {
    while (*cmd_raw == ' ' || *cmd_raw == '\t') cmd_raw++;
    size_t c_len = strlen(cmd_raw);
    while (c_len > 0 && (cmd_raw[c_len-1] == ' ' || cmd_raw[c_len-1] == '\t')) {
      cmd_raw[--c_len] = '\0';
    }
    if (strlen(cmd_raw) == 0 || strncmp(cmd_raw, "none", 5) == 0) {
      cmd_raw = NULL;
    }
  }

  if (strlen(target_raw) == 0) {
    free(val_copy);
    return;
  }

  char *template_name = strdup(key);
  char *target_path = strdup(target_raw);
  char *reload_cmd = cmd_raw ? strdup(cmd_raw) : NULL;
  if (!template_name || !target_path || (cmd_raw && !reload_cmd)) {
    free(template_name);
    free(target_path);
    free(reload_cmd);
    free(val_copy);
    return;
  }

  // Store the link
  Link *new_links = realloc(config->links, sizeof(Link) * (config->num_links + 1));
  if (new_links) {
    config->links = new_links;
    config->links[config->num_links].template_name = template_name;
    config->links[config->num_links].target_path = target_path;
    config->links[config->num_links].reload_cmd = reload_cmd;
    config->num_links++;
  } else {
    free(template_name);
    free(target_path);
    free(reload_cmd);
  }

  free(val_copy);
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
  char *cache_home = get_cache_home();
  config->out_dir = cache_home ? build_path(cache_home, "cwal") : NULL;
  free(cache_home);
  if (!config->out_dir) {
    config->out_dir = strdup("~/.cache/cwal");
  }
  if (!config->out_dir) {
    perror("Failed to allocate out_dir");
    free(config);
    return NULL;
  }
  config->backend = NULL;
  config->mode = DARK;
  config->cols16_mode = DARKEN;
  config->alpha = 1.0;
  config->saturation = 0.0;
  config->contrast = 1.0;
  config->script_path = NULL;
  config->random_dir = NULL;
  config->links = NULL;
  config->num_links = 0;

  char *config_home = get_config_home();
  char *expanded_path = build_path(config_home, "cwal", "cwal.ini");
  FILE *file = fopen(expanded_path, "r");
  if (!file) {
    logging(WARN, "Config file not found (%s), using default values.", expanded_path);
    free(config_home);
    free(expanded_path);
    return config;
  }
  free(config_home);

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
      size_t sect_len = len - 2;
      if (sect_len >= sizeof(section))
        sect_len = sizeof(section) - 1;
      memcpy(section, trimmed_line + 1, sect_len);
      section[sect_len] = '\0';
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

        if (strncmp(section, "links", 5) == 0) {
          parse_link(config, key, value);
        } else {
          parse_key_value(config, key, value);
        }
      }
    }
  }

  fclose(file);
  free(expanded_path);
  return config;
}

void save_config(const Config *config) {
  char *config_home = get_config_home();
  char *config_dir_path = build_path(config_home, "cwal");
  char *expanded_path = build_path(config_dir_path, "cwal.ini");

  if (validate_or_create_dir(config_dir_path) != 0) {
    logging(ERROR, "Failed to create config directory: %s", config_dir_path);
    free(config_dir_path);
    free(config_home);
    free(expanded_path);
    return;
  }
  create_config_subdirectories(config_dir_path);

  FILE *file = fopen(expanded_path, "w");
  if (!file) {
    logging(ERROR, "Failed to open config file for writing: %s", expanded_path);
    free(config_dir_path);
    free(config_home);
    free(expanded_path);
    return;
  }

  fprintf(file, "[general]\n");
  fprintf(file, "out_dir = %s\n", config->out_dir ? config->out_dir : "");
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

  if (config->num_links > 0) {
    fprintf(file, "\n[links]\n");
    for (int i = 0; i < config->num_links; i++) {
      if (config->links[i].reload_cmd && strlen(config->links[i].reload_cmd) > 0) {
        fprintf(file, "%s = %s | %s\n",
                config->links[i].template_name,
                config->links[i].target_path ? config->links[i].target_path : "",
                config->links[i].reload_cmd);
      } else {
        fprintf(file, "%s = %s\n",
                config->links[i].template_name,
                config->links[i].target_path ? config->links[i].target_path : "");
      }
    }
  }

  fclose(file);
  logging(INFO, "Config saved to %s", expanded_path);
  free(config_dir_path);
  free(config_home);
  free(expanded_path);
}

void free_config(Config *config) {
  if (config) {
    free(config->out_dir);
    free(config->backend);
    free(config->script_path);
    free(config->random_dir);
    for (int i = 0; i < config->num_links; i++) {
      free(config->links[i].template_name);
      free(config->links[i].target_path);
      free(config->links[i].reload_cmd);
    }
    free(config->links);
    free(config);
  }
}
