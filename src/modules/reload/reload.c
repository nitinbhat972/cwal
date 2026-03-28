/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "reload.h"
#include "utils/path.h"
#include "utils/utils.h"
#include <dirent.h>
#include <fcntl.h>
#include <glob.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

#define MAX_DEVICES 256
#define LINE_BUFFER_SIZE 256

static int command_exists(const char *cmd) {
  char *path_env = getenv("PATH");
  if (!path_env) return 0;
  char *path_copy = strdup(path_env);
  if (!path_copy)
    return 0;

  char *saveptr;
  char *dir = strtok_r(path_copy, ":", &saveptr);
  while (dir != NULL) {
    char *full_path = build_path(dir, cmd);
    if (access(full_path, X_OK) == 0) {
      free(full_path);
      free(path_copy);
      return 1;
    }
    free(full_path);
    dir = strtok_r(NULL, ":", &saveptr);
  }
  free(path_copy);
  return 0;
}

static void broadcast_to_terminals(const char *sequences, size_t len) {
  static struct utsname sys_info;
  uname(&sys_info);
  const char *os = sys_info.sysname;

  glob_t glob_result;
  char **devices = NULL;
  int device_count = 0;

  if (strncmp(os, "Darwin", 7) == 0) {
    if (glob("/dev/ttys00[0-9]*", 0, NULL, &glob_result) == 0) {
      devices = glob_result.gl_pathv;
      device_count = glob_result.gl_pathc;
    }
  } else {
    if (glob("/dev/pts/[0-9]*", 0, NULL, &glob_result) == 0) {
      devices = glob_result.gl_pathv;
      device_count = glob_result.gl_pathc;
    }
  }

  if (devices && device_count > 0) {
    for (int i = 0; i < device_count; i++) {
      int fd = open(devices[i], O_WRONLY | O_NOCTTY | O_NONBLOCK);
      if (fd != -1) {
        write(fd, sequences, len);
        close(fd);
      }
    }
  }

  if (strncmp(os, "OpenBSD", 8) != 0) {
    globfree(&glob_result);
  }
}

static char *read_file_to_buffer(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;

  fseek(f, 0, SEEK_END);
  *size = ftell(f);
  fseek(f, 0, SEEK_SET);

  if (*size == 0) {
      fclose(f);
      return NULL;
  }

  char *buffer = malloc(*size + 1);
  if (buffer) {
    size_t read_bytes = fread(buffer, 1, *size, f);
    buffer[read_bytes] = '\0';
  }
  fclose(f);
  return buffer;
}

static void sync_file(const char *src_path, const char *dest_path) {
  size_t src_size;
  char *src_content = read_file_to_buffer(src_path, &src_size);
  if (!src_content) return;

  char *dest_copy = strdup(dest_path);
  if (dest_copy) {
    char *last_slash = strrchr(dest_copy, '/');
    if (last_slash) {
      *last_slash = '\0';
      if(validate_or_create_dir(dest_copy) != 0){
        logging(WARN,"Failed to create the dir: %s", dest_copy);
      }
    }
    free(dest_copy);
  }

  size_t dest_size;
  char *dest_content = read_file_to_buffer(dest_path, &dest_size);

  if (dest_content) {
    char *start_marker = strstr(dest_content, "$CWAL_START");
    char *end_marker = strstr(dest_content, "$CWAL_END");

    if (start_marker && end_marker && end_marker > start_marker) {
      // Surgical Injection
      logging(INFO, "Injecting colors into %s", dest_path);
      
      FILE *f = fopen(dest_path, "wb");
      if (f) {
        char *start_write_pos = strchr(start_marker, '\n');
        if (!start_write_pos) start_write_pos = start_marker + strlen("$CWAL_START");
        else start_write_pos++;

        char *end_write_pos = end_marker;
        while (end_write_pos > dest_content && *(end_write_pos - 1) != '\n') {
          end_write_pos--;
        }

        fwrite(dest_content, 1, start_write_pos - dest_content, f);
        
        fwrite(src_content, 1, src_size, f);
        if (src_content[src_size-1] != '\n') fputc('\n', f);

        fwrite(end_write_pos, 1, strlen(end_write_pos), f);
        fclose(f);
      }
      free(src_content);
      free(dest_content);
      return;
    }
  }
  free(dest_content);

  // Full Overwrite
  char backup_path[PATH_MAX];
  snprintf(backup_path, sizeof(backup_path), "%s.bak", dest_path);
  if (access(dest_path, F_OK) == 0 && access(backup_path, F_OK) != 0) {
    rename(dest_path, backup_path);
  }

  logging(INFO, "Replacing file: %s", dest_path);
  FILE *f = fopen(dest_path, "wb");
  if (f) {
    fwrite(src_content, 1, src_size, f);
    fclose(f);
  }
  free(src_content);
}



void apply_colors_to_apps(const char *out_dir, Config *config, bool no_reload) {
  if (no_reload) {
    logging(INFO, "Skipping application reload as --no-reload was specified.");
    return;
  }

  // 1. Terminal Sequences
  char *sequences_path = build_path(out_dir, "sequences");
  size_t seq_len;
  char *sequences = read_file_to_buffer(sequences_path, &seq_len);
  if (sequences) {
    broadcast_to_terminals(sequences, seq_len);
    fprintf(stdout, "%s", sequences);
    fflush(stdout);
    free(sequences);
  }
  free(sequences_path);

  // 2. Legacy TTY
  char *tty_script_path = build_path(out_dir, "colors-tty.sh");
  char *term_env = getenv("TERM");
  if (term_env && strncmp(term_env, "linux", 6) == 0 && access(tty_script_path, F_OK) == 0) {
    char command[PATH_MAX + 10];
    snprintf(command, sizeof(command), "sh %s", tty_script_path);
    execute_command(command);
  }
  free(tty_script_path);

  // 3. Xresources
  char *xrdb_path = build_path(out_dir, "colors.Xresources");
  if (command_exists("xrdb") && access(xrdb_path, F_OK) == 0) {
    char command[PATH_MAX + 30];
    snprintf(command, sizeof(command), "xrdb -merge -quiet %s", xrdb_path);
    execute_command(command);
  }
  free(xrdb_path);

  // 4. Dynamic Links & Reloads
  for (int i = 0; i < config->num_links; i++) {
    char *src = build_path(out_dir, config->links[i].template_name);
    sync_file(src, config->links[i].target_path);
    if (config->links[i].reload_cmd) {
      logging(INFO, "Running reload command: %s", config->links[i].reload_cmd);
      execute_command(config->links[i].reload_cmd);
    }
    free(src);
  }

  logging(INFO, "Finished applying colors to applications.");
}
