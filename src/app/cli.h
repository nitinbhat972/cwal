#pragma once

#include "core.h"
#include "config.h"
#include "modules/theme/themes.h"

typedef struct {
    char *image_path;       // Path to the wallpaper image.
    COLOR_MODE mode;        // Theme mode (dark or light).
    SHADE_MODE cols16_mode; // 16-color generation mode (darken or lighten).
    float saturation;       // Overall saturation level.
    float contrast;         // Contrast ratio.
    float alpha;            // Alpha transparency value.
    char *backend;          // Name of the image processing backend to use.
    char *script_path;      // Path to a script to run after processing.
    char *out_dir;          // Output directory for generated files.
    bool no_reload;         // Flag to prevent reloading applications.
    bool list_backends;     // Flag to list available backends.
    bool list_themes;       // Flag to list available themes.
    bool quiet;             // Flag to suppress all output.
    char *random_dir;       // Path to a directory for random image selection.
    RandomMode random_mode; // Mode for random theme selection.
    char *theme;            // Name of the theme to load.
    bool preview;           // show palette preview.
} CliArgs;

int parse_cli_args(int argc, char **argv, Config *config, CliArgs *args);
void free_cli_args(CliArgs *args);
void print_usage(const char *prog_name);
