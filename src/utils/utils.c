/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "utils.h"
#include "color/color_operation.h"
#include "core.h"
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static bool quiet_mode = false;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

float clamp_value(float amount) {
  if (amount < 0.0f)
    return 0.0f;
  if (amount > 1.0f)
    return 1.0f;
  return amount;
}

uint8_t clamp_byte(float value) {
  if (value < 0.0f)
    return 0;
  if (value > 255.0f)
    return 255;
  return (uint8_t)(value + 0.5f);
}

static inline int compare_by_luminance(const void *a, const void *b) {
  const Color *c1 = (const Color *)a;
  const Color *c2 = (const Color *)b;

  float l1 = w3_luminance(*c1);
  float l2 = w3_luminance(*c2);

  if (l1 < l2)
    return -1;
  if (l1 > l2)
    return 1;
  return 0;
}

void sort_color(Palette *palette) {
  qsort(palette->colors, PALETTE_MAX_SIZE, sizeof(Color), compare_by_luminance);
}

void preview_palette() {
  printf("\n");
  for (int i = 0; i < 16; i++) {
    printf("\033[48;5;%dm    \033[0m", i);
    if (i % 8 == 7)
      printf("\n");
  }
  printf("\n");
}

void set_quiet_mode(bool quiet) { quiet_mode = quiet; }

int execute_command(const char *command) {
  if (!command || strlen(command) == 0)
    return 0;

  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    char *cmd_copy = strdup(command);
    char *args[64];
    int i = 0;
    char *saveptr;
    char *token = strtok_r(cmd_copy, " ", &saveptr);

    while (token != NULL && i < 63) {
      args[i++] = token;
      token = strtok_r(NULL, " ", &saveptr);
    }
    args[i] = NULL;

    if (args[0] == NULL) {
      free(cmd_copy);
      exit(0);
    }

    execvp(args[0], args);
    perror("execvp");
    free(cmd_copy);
    exit(1);
  } else if (pid > 0) {
    // Parent process
    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
  } else {
    perror("fork");
    return -1;
  }
}

void logging(int log_level, const char *format, ...) {
  if (quiet_mode) {
    return;
  }

  pthread_mutex_lock(&log_mutex);

  static const char *color[] = {BLUE, YELLOW, RED};
  static const char type[] = {'I', 'W', 'E'};
  static const int max_level = sizeof(type) / sizeof(type[0]);

  if (log_level < 0 || log_level >= max_level)
    log_level = 0;

  va_list args;
  va_start(args, format);
  printf("[%s%c%s]: ", color[log_level], type[log_level], RESET);
  vprintf(format, args);
  printf("\n");
  va_end(args);

  pthread_mutex_unlock(&log_mutex);
}
