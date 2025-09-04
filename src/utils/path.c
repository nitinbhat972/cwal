#include "path.h"
#include "utils.h"
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

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

  const char *dir = expand_home(dir_in);
  if (!dir) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (mkdir_p(dir, 0700) != 0 && errno != EEXIST) {
    perror("Failed to create directory");
    return -1;
  }

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
    to_lowercase(name);
    if (strstr(name, ".jpg") || strstr(name, ".jpeg") || strstr(name, ".png") ||
        strstr(name, ".gif")) {
      count++;
      image_files = realloc(image_files, sizeof(char *) * count);
      if (!image_files) {
        logging(ERROR, "Memory allocation failed for image files.\n");
        free(name);
        closedir(d);
        free(directory);
        for (int i = 0; i < count - 1; i++) {
          free(image_files[i]);
        }
        free(image_files);
        return NULL;
      }
      image_files[count - 1] = name;
    } else {
      free(name);
    }
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

char *normalize_cli_path(const char *path) {
  char *expanded_path = expand_home(path);
  if (!expanded_path)
    return NULL;

  char *resolved_path = realpath(expanded_path, NULL);
  if (resolved_path) {
    free(expanded_path);
    return resolved_path;
  }

  return expanded_path;
}

char *build_path(const char *path1, const char *path2) {
  size_t len1 = strlen(path1);
  size_t len2 = strlen(path2);
  char *new_path = malloc(len1 + len2 + 2);
  if (!new_path) {
    logging(ERROR, "Memory allocation failed for build_path.\n");
    return NULL;
  }
  snprintf(new_path, len1 + len2 + 2, "%s", path1);
  if (new_path[len1 - 1] != '/' && path2[0] != '/') {
    snprintf(new_path + strlen(new_path), len1 + len2 + 2 - strlen(new_path),
             "%s", "/");
  }
  snprintf(new_path + strlen(new_path), len1 + len2 + 2 - strlen(new_path),
           "%s", path2);
  return new_path;
}
