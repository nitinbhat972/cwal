/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "path.h"
#include "utils.h"
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static bool has_supported_image_extension(const char *filename) {
  const char *extension = strrchr(filename, '.');
  if (!extension) {
    return false;
  }

  return strcasecmp(extension, ".jpg") == 0 ||
         strcasecmp(extension, ".jpeg") == 0 ||
         strcasecmp(extension, ".png") == 0 ||
         strcasecmp(extension, ".gif") == 0;
}

char *expand_home(const char *path) {
  if (!path)
    return NULL;

  if (path[0] == '~') {
    const char *home = getenv("HOME");
    if (!home)
      home = "/tmp";

    size_t len = strlen(home) + strlen(path + 1) + 1;
    char *out = malloc(len);
    if (!out)
      return NULL;
    snprintf(out, len, "%s%s", home, path + 1);
    return out;
  }

  return strdup(path);
}

char *get_config_home(void) {
  char *xdg_config = getenv("XDG_CONFIG_HOME");
  if (xdg_config && strlen(xdg_config) > 0) {
    return strdup(xdg_config);
  }
  return expand_home("~/.config");
}

char *get_cache_home(void) {
  char *xdg_cache = getenv("XDG_CACHE_HOME");
  if (xdg_cache && strlen(xdg_cache) > 0) {
    return strdup(xdg_cache);
  }
  return expand_home("~/.cache");
}

char *get_data_home(void) {
  char *xdg_data = getenv("XDG_DATA_HOME");
  if (xdg_data && strlen(xdg_data) > 0) {
    return strdup(xdg_data);
  }
  return expand_home("~/.local/share");
}

char **get_data_dirs(void) {
  const char *env = getenv("XDG_DATA_DIRS");
  if (!env || strlen(env) == 0) {
    env = "/usr/local/share:/usr/share";
  }

  char *env_copy = strdup(env);
  if (!env_copy)
    return NULL;

  int count = 0;
  char *temp = env_copy;
  while (*temp) {
    if (*temp == ':')
      count++;
    temp++;
  }
  count++; // Add one for the last element

  char **dirs = malloc(sizeof(char *) * (count + 1));
  if (!dirs) {
    free(env_copy);
    return NULL;
  }

  int i = 0;
  char *saveptr;
  char *token = strtok_r(env_copy, ":", &saveptr);
  while (token != NULL) {
    char *dup = strdup(token);
    if (!dup) {
      for (int j = 0; j < i; j++) {
        free(dirs[j]);
      }
      free(dirs);
      free(env_copy);
      return NULL;
    }
    dirs[i++] = dup;
    token = strtok_r(NULL, ":", &saveptr);
  }
  dirs[i] = NULL;

  free(env_copy);
  return dirs;
}

static int mkdir_p(const char *path, mode_t mode) {
  char tmp[PATH_MAX];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp), "%s", path);
  len = strlen(tmp);

  if (tmp[len - 1] == '/') {
    tmp[len - 1] = 0;
  }

  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;
      if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
        return -1;
      }
      *p = '/';
    }
  }

  return mkdir(tmp, mode);
}

int validate_or_create_dir(const char *dir_in) {
  if (!dir_in) {
    errno = EINVAL;
    logging(ERROR, "validate_or_create_dir: NULL input\n");
    return -1;
  }

  char *dir = expand_home(dir_in);
  if (!dir) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (mkdir_p(dir, 0700) != 0 && errno != EEXIST) {
    logging(ERROR, "Failed to create directory: %s", dir_in);
    free(dir);
    return -1;
  }

  free(dir);
  return 0;
}

char *get_random_image_path(const char *directory_in) {
  char *directory = expand_home(directory_in);
  if (!directory) {
    logging(ERROR, "Failed to expand home directory for: %s", directory_in);
    return NULL;
  }

  DIR *d;
  struct dirent *dir;
  char **image_files = NULL;
  int count = 0;
  char *random_image_path = NULL;

  d = opendir(directory);
  if (!d) {
    logging(ERROR, "Could not open directory: %s\n", directory);
    free(directory);
    return NULL;
  }

  while ((dir = readdir(d)) != NULL) {
    if (!has_supported_image_extension(dir->d_name)) {
      continue;
    }

    char *name = strdup(dir->d_name);
    if (!name) {
      logging(ERROR, "Memory allocation failed for file name.\n");
      closedir(d);
      free(directory);
      for (int i = 0; i < count; i++) {
        free(image_files[i]);
      }
      free(image_files);
      return NULL;
    }

    char **resized = realloc(image_files, sizeof(char *) * (count + 1));
    if (!resized) {
      logging(ERROR, "Memory allocation failed for image files.\n");
      free(name);
      closedir(d);
      free(directory);
      for (int i = 0; i < count; i++) {
        free(image_files[i]);
      }
      free(image_files);
      return NULL;
    }

    image_files = resized;
    image_files[count] = name;
    count++;
  }
  closedir(d);

  if (count == 0) {
    logging(ERROR, "No image files found in directory: %s\n", directory);
    free(directory);
    return NULL;
  }

  unsigned int seed = time(NULL);
  int random_index = rand_r(&seed) % count;
  char *selected_file = image_files[random_index];

  random_image_path = build_path(directory, selected_file);
  if (!random_image_path) {
    logging(ERROR, "Memory allocation failed for random image path.");
  }

  for (int i = 0; i < count; i++) {
    free(image_files[i]);
  }
  free(image_files);
  free(directory);

  return random_image_path;
}

char *build_path_internal(const char *first, ...) {
  if (!first)
    return NULL;

  const char *separator = "/";
  size_t total_len = strlen(first);

  va_list args;
  va_start(args, first);
  const char *next = va_arg(args, const char *);
  while (next) {
    total_len += strlen(separator) + strlen(next);
    next = va_arg(args, const char *);
  }
  va_end(args);

  char *result = malloc(total_len + 1);
  if (!result) {
    logging(ERROR, "Memory allocation failed for build_path.\n");
    return NULL;
  }

  strcpy(result, first);
  size_t res_len = strlen(result);
  if (res_len > 0 && result[res_len - 1] == '/') {
    result[--res_len] = '\0';
  }

  va_start(args, first);
  next = va_arg(args, const char *);
  while (next) {
    if (strlen(next) == 0) {
      next = va_arg(args, const char *);
      continue;
    }

    strcat(result, separator);
    
    const char *to_add = next;
    if (to_add[0] == '/') {
      to_add++;
    }
    strcat(result, to_add);

    res_len = strlen(result);
    if (res_len > 0 && result[res_len - 1] == '/') {
      result[--res_len] = '\0';
    }

    next = va_arg(args, const char *);
  }
  va_end(args);

  return result;
}
