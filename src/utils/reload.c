#include "reload.h"
#include "path.h"
#include "utils.h"
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

static int command_exists(const char *cmd) {
  char *path_copy = strdup(getenv("PATH"));
  if (!path_copy)
    return 0;

  char *dir = strtok(path_copy, ":");
  while (dir != NULL) {
    char *full_path = build_path(dir, cmd); // Remove the array declaration
    if (access(full_path, X_OK) == 0) {
      free(full_path);
      free(path_copy);
      return 1;
    }
    free(full_path);
    dir = strtok(NULL, ":");
  }
  free(path_copy);
  return 0;
}

static int get_pid(const char *process) {
  DIR *dir = opendir("/proc");
  if (!dir)
    return 0;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    char *endptr;
    long pid = strtol(entry->d_name, &endptr, 10);
    if (*endptr != '\0') // Not a pid directory
      continue;

    char *path = build_path("/proc", entry->d_name);
    char *cmdline_path = build_path(path, "cmdline");
    FILE *file = fopen(cmdline_path, "r");
    if (file) {
      char cmdline[256];
      if (fgets(cmdline, sizeof(cmdline), file)) {
        if (strstr(cmdline, process)) {
          fclose(file);
          closedir(dir);
          free(path);
          free(cmdline_path);
          return (int)pid; // return actual pid
        }
      }
      fclose(file);
    }
    free(path);
    free(cmdline_path);
  }
  closedir(dir);
  return 0; // Process not found
}

static const char *get_os() {
  static struct utsname sys_info;
  static int initialized = 0;

  if (!initialized) {
    if (uname(&sys_info) == 0) {
      initialized = 1;
    } else {
      // If uname fails, set a default value
      strcpy(sys_info.sysname, "Unknown");
      initialized = 1;
    }
  }

  return sys_info.sysname;
}

static void broadcast_to_terminals(const char *sequences, size_t len) {
  const char *os = get_os();
  glob_t glob_result;
  char **devices = NULL;
  int device_count = 0;

  if (strcmp(os, "Darwin") == 0) {
    // macOS: Use /dev/ttys00[0-9]*
    if (glob("/dev/ttys00[0-9]*", 0, NULL, &glob_result) == 0) {
      devices = glob_result.gl_pathv;
      device_count = glob_result.gl_pathc;
    }
  } else if (strcmp(os, "OpenBSD") == 0) {
    // OpenBSD: Use ps command output
    FILE *ps_output =
        popen("ps -o tty | sed -e 1d -e s#^#/dev/# | sort | uniq", "r");
    if (ps_output) {
      char line[256];
      char temp_devices[100][256];
      device_count = 0;

      while (fgets(line, sizeof(line), ps_output) && device_count < 100) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0) {
          strcpy(temp_devices[device_count], line);
          device_count++;
        }
      }
      pclose(ps_output);

      // Convert to array of pointers
      if (device_count > 0) {
        devices = malloc(device_count * sizeof(char *));
        for (int i = 0; i < device_count; i++) {
          devices[i] = temp_devices[i];
        }
      }
    }
  } else {
    // Linux and other Unix-like systems: Use /dev/pts/[0-9]*
    if (glob("/dev/pts/[0-9]*", 0, NULL, &glob_result) == 0) {
      devices = glob_result.gl_pathv;
      device_count = glob_result.gl_pathc;
    }
  }

  // Send sequences to discovered devices
  if (devices && device_count > 0) {
    for (int i = 0; i < device_count; i++) {
      const char *dev = devices[i];

      // Skip /dev/pts/0 on KDE Plasma
      if (strcmp(dev, "/dev/pts/0") == 0) {
        char *desktop_session = getenv("DESKTOP_SESSION");
        if (desktop_session && strcmp(desktop_session, "plasma") == 0) {
          continue;
        }
      }

      int fd = open(dev, O_WRONLY | O_NOCTTY);
      if (fd != -1) {
        write(fd, sequences, len);
        close(fd);
      }
    }
  }

  if (strcmp(os, "Darwin") == 0 || strcmp(os, "Linux") == 0) {
    globfree(&glob_result);
  } else if (strcmp(os, "OpenBSD") == 0 && devices) {
    free(devices);
  }
}

void apply_colors_to_apps(const char *out_dir, bool no_reload) {
  if (no_reload) {
    logging(INFO, "Skipping application reload as --no-reload was specified.");
    return;
  }

  logging(INFO, "Applying colors to applications...");

  char *sequences_path = build_path(out_dir, "sequences");

  // 1. Apply terminal sequences to ALL open terminals
  if (access(sequences_path, F_OK) == 0) {
    FILE *seq_file = fopen(sequences_path, "r");
    if (seq_file) {
      fseek(seq_file, 0, SEEK_END);
      long file_size = ftell(seq_file);
      fseek(seq_file, 0, SEEK_SET);

      char *sequences = malloc(file_size + 1);
      if (sequences) {
        size_t bytes_read = fread(sequences, 1, file_size, seq_file);
        sequences[bytes_read] = '\0';

        // Broadcast to all user PTYs
        broadcast_to_terminals(sequences, bytes_read);

        // Apply to current terminal too
        fprintf(stdout, "%s", sequences);
        fflush(stdout);

        free(sequences);
        logging(INFO, "Applied terminal sequences to all open terminals.");
      } else {
        logging(WARN, "Failed to allocate memory for sequences.");
      }
      fclose(seq_file);
    } else {
      logging(WARN, "Could not open terminal sequences file: %s",
              sequences_path);
    }
  } else {
    logging(WARN, "Terminal sequences file not found: %s", sequences_path);
  }
  free(sequences_path);

  // 2. tty
  char *tty_script_path = build_path(out_dir, "colors-tty.sh");
  char *term_env = getenv("TERM");
  if (term_env && strcmp(term_env, "linux") == 0 &&
      access(tty_script_path, F_OK) == 0) {
    char command[PATH_MAX + 10];
    snprintf(command, sizeof(command), "sh %s", tty_script_path);
    system(command);
    logging(INFO, "Applied TTY colors.");
  } else {
    logging(INFO,
            "Skipping TTY colors (TERM not 'linux' or script not found).");
  }
  free(tty_script_path);

  // 3. xrdb
  char *xrdb_path = build_path(out_dir, "colors.Xresources");
  if (command_exists("xrdb") && access(xrdb_path, F_OK) == 0) {
    char command[PATH_MAX + 20];
    snprintf(command, sizeof(command), "xrdb -merge -quiet %s", xrdb_path);
    system(command);
    logging(INFO, "Applied Xresources.");
  }
  free(xrdb_path);

  // 4. i3
  if (command_exists("i3-msg") && get_pid("i3") != 0) {
    system("i3-msg reload");
    logging(INFO, "Reloaded i3.");
  }

  // 5. bspwm
  if (command_exists("bspc") && get_pid("bspwm") != 0) {
    system("bspc wm -r");
    logging(INFO, "Reloaded bspwm.");
  }

  // 6. polybar
  if (command_exists("polybar") && get_pid("polybar") != 0) {
    system("pkill -USR1 polybar");
    logging(INFO, "Reloaded Polybar.");
  }

  // 7. sway
  if (command_exists("swaymsg") && get_pid("sway") != 0) {
    system("swaymsg reload");
    logging(INFO, "Reloaded Sway.");
  }

  // 8. waybar
  if (command_exists("waybar") && get_pid("waybar") != 0) {
    system("pkill -USR2 waybar");
    logging(INFO, "Reloaded Waybar.");
  }

  // 9. mako (Wayland notification daemon)
  if (command_exists("makoctl") && get_pid("mako") != 0) {
    system("makoctl reload");
    logging(INFO, "Reloaded Mako.");
  }

  // 10. nvim (nvim-colo-reload)
  if (command_exists("nvim-colo-reload") && get_pid("nvim") != 0) {
    system("nvim-colo-reload");
    logging(INFO, "Reloaded Neovim colors.");
  }

  // 11. termux
  if (command_exists("termux-reload-settings")) {
    system("termux-reload-settings");
    logging(INFO, "Reloaded Termux settings.");
  }

  logging(INFO, "Finished applying colors to applications.");
}
