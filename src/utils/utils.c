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
#include <fcntl.h>

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

void reverse_colors(Palette *palette) {
  if (!palette) return;
  for (int i = 0; i < 4; i++) {
    Color temp = palette->colors[i];
    palette->colors[i] = palette->colors[7 - i];
    palette->colors[7 - i] = temp;
  }
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

char *replace_placeholder(const char *str, const char *old, const char *new_str) {
  if (!str || !old || !new_str) return str ? strdup(str) : NULL;

  char *result;
  int i, count = 0;
  size_t oldlen = strlen(old);
  size_t newlen = strlen(new_str);

  for (i = 0; str[i] != '\0'; i++) {
    if (strstr(&str[i], old) == &str[i]) {
      count++;
      i += oldlen - 1;
    }
  }

  result = (char *)malloc(i + count * (newlen - oldlen) + 1);
  if (!result) return NULL;

  i = 0;
  while (*str) {
    if (strstr(str, old) == str) {
      strcpy(&result[i], new_str);
      i += newlen;
      str += oldlen;
    } else {
      result[i++] = *str++;
    }
  }
  result[i] = '\0';
  return result;
}

int execute_command(const char *command) {
  if (!command || strlen(command) == 0)
    return 0;

  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    int fd = open("/dev/null", O_WRONLY);
    if (fd != -1) {
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

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
