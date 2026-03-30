/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "app/cli.h"
#include "app/config.h"
#include "backends/backend.h"
#include "color/colors.h"
#include "core.h"
#include "modules/cache/cache.h"
#include "modules/reload/reload.h"
#include "modules/template/template.h"
#include "modules/theme/themes.h"
#include "utils/path.h"
#include "utils/utils.h"
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argv, char **argc) {
  // Load config file
  Config *app_config = load_config();
  if (!app_config) {
    logging(ERROR, "Failed to load configuration.");
    return 1;
  }

  // Parse command-line arguments
  CliArgs args = {0};
  CliStatus cli_status = parse_cli_args(argv, argc, app_config, &args);
  if (cli_status == CLI_ERROR) {
    free_cli_args(&args);
    free_config(app_config);
    return 1;
  }

  if (cli_status == CLI_EXIT) {
    free_cli_args(&args);
    free_config(app_config);
    return 0;
  }

  set_quiet_mode(args.quiet);

  if (args.list_backends) {
    init_backends();
    list_all_backends();
    free_config(app_config);
    free_cli_args(&args);
    return 0;
  }

  if (args.list_themes) {
    list_themes();
    free_config(app_config);
    free_cli_args(&args);
    return 0;
  }

  if (args.preview) {
    logging(INFO, "Current colorscheme:\n");
    preview_palette();
    free_config(app_config);
    free_cli_args(&args);
    return 0;
  }

  // Initialize backends
  init_backends();

  // Palette structure initiallation
  Palette palette = {0};
  palette.mode = args.mode;
  palette.cols16_mode = args.cols16_mode;
  palette.saturation = args.saturation;
  palette.contrast = args.contrast;
  palette.alpha = args.alpha;

  if (args.use_random_theme) {
    if (load_random_theme(&palette, args.random_mode) != 0) {
      free_config(app_config);
      free_cli_args(&args);
      return -1;
    }
    palette.cols16_mode = NONE;
  } else if (args.theme) {
    if (load_theme(&palette, args.theme) != 0) {
      free_config(app_config);
      free_cli_args(&args);
      return -1;
    }
    palette.cols16_mode = NONE;
  } else {
    if (palette.cols16_mode == NONE) {
      palette.cols16_mode = DARKEN;
    }

    char *image_to_process_path = NULL;
    if (args.use_random_dir) {
      image_to_process_path = get_random_image_path(args.random_dir);
      if (!image_to_process_path) {
        free_config(app_config);
        free_cli_args(&args);
        return -1;
      }
      logging(INFO, "Selected random image: %s", image_to_process_path);
    } else if (args.image_path) {
      image_to_process_path = expand_home(args.image_path);
      if (!image_to_process_path) {
        logging(ERROR, "Failed to resolve image path.");
        free_config(app_config);
        free_cli_args(&args);
        return -1;
      }
    }

    const char *path = image_to_process_path;
    if (!path) {
      logging(ERROR, "Failed to resolve image path.");
      free_config(app_config);
      free_cli_args(&args);
      return -1;
    }

    // Selects backend
    ImageBackend *backend = backend_get(args.backend);
    if (!backend) {
      logging(ERROR, "Backend not found!");
      free(image_to_process_path);
      free_config(app_config);
      free_cli_args(&args);
      return -1;
    }

    // Apply settings from CLI
    palette.wallpaper = image_to_process_path;
    image_to_process_path = NULL;
    // Loads colors from cache
    if (load_palette_from_cache(&palette, args.out_dir, args.backend) != 0) {
      // Use unified backend processing with fallback mechanism
      if (process_with_fallback(backend, path, &palette) != 0) {
        logging(ERROR, "All backends failed to process the image!");
        free(palette.wallpaper);
        palette.wallpaper = NULL;
        free_config(app_config);
        free_cli_args(&args);
        return -1;
      }
      process_colors(&palette, args.saturation, args.contrast);
      save_palette_to_cache(&palette, args.out_dir, args.backend);
    }
  }

  // Generates template files
  process_template(args.out_dir, &palette);

  // Reloads the app colors
  apply_colors_to_apps(args.out_dir, app_config, args.no_reload);

  // Runs post hooks
  if (args.script_path) {
    char *resolved_script_path = expand_home(args.script_path);
    if (resolved_script_path) {
      char *final_cmd = replace_placeholder(
          resolved_script_path, "$current_wallpaper",
          palette.wallpaper ? palette.wallpaper : "");
      if (final_cmd) {
        logging(INFO, "Running post-hook script: %s", final_cmd);
        execute_command(final_cmd);
        free(final_cmd);
      }
      free(resolved_script_path);
    } else {
      logging(ERROR, "Failed to resolve post-hook script path.");
    }
  }

  free(palette.wallpaper);
  palette.wallpaper = NULL;

  // Updates the config file to the current values
  free(app_config->out_dir);
  app_config->out_dir = strdup(args.out_dir);
  app_config->mode = palette.mode;
  app_config->cols16_mode = palette.cols16_mode;
  app_config->alpha = palette.alpha;
  app_config->saturation = palette.saturation;
  app_config->contrast = palette.contrast;

  free(app_config->backend);
  app_config->backend = strdup(args.backend);

  free(app_config->script_path);
  app_config->script_path = args.script_path ? strdup(args.script_path) : NULL;

  if (args.random_dir) {
    free(app_config->random_dir);
    app_config->random_dir = strdup(args.random_dir);
  }

  // Saves the config
  save_config(app_config);

  // Prints the generated color palatte
  preview_palette();

  free_config(app_config);
  free_cli_args(&args);
  return 0;
}
