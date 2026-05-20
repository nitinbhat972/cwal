/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#pragma once

#include "config.h"
#include "modules/theme/themes.h"

typedef struct {
    AppOptions  opts;           // Options from AppOptions
    char       *image_path;     // Path to the wallpaper image.
    bool        backend_specified; // Flag: backend was explicitly set via CLI.
    bool        no_reload;      // Flag to prevent reloading applications.
    bool        list_backends;  // Flag to list available backends.
    bool        list_themes;    // Flag to list available themes.
    bool        quiet;          // Flag to suppress all output.
    bool        use_random_dir; // Flag to use random image selection.
    bool        use_random_theme; // Flag to use a random theme.
    RandomMode  random_mode;    // Mode for random theme selection.
    char       *theme;          // Name of the theme to load.
    bool        preview;        // Show palette preview.
} CliArgs;

typedef enum {
    CLI_OK,
    CLI_ERROR,
    CLI_EXIT
} CliStatus;

CliStatus parse_cli_args(int argc, char **argv, Config *config, CliArgs *args);
void free_cli_args(CliArgs *args);
void print_usage(const char *prog_name);
