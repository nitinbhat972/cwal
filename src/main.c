#include "backend.h"
#include "cache.h"
#include "cli.h"
#include "colors.h"
#include "config.h"
#include "core.h"
#include "hooks.h"
#include "image.h"
#include "path.h"
#include "reload.h"
#include "template.h"
#include "utils.h"
#include <dirent.h>
#include <linux/limits.h>
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
  CliArgs args;
  if (parse_cli_args(argv, argc, app_config, &args) != 0) {
    free_config(app_config);
    return 1;
  }

  set_quiet_mode(args.quiet);

  if (args.list_backends) {
    list_all_backends();
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

  char *image_to_process_path = NULL;
  if (args.random_dir) {
    image_to_process_path = get_random_image_path(args.random_dir);
    if (!image_to_process_path) {
      free_config(app_config);
      free_cli_args(&args);
      return 1;
    }
    logging(INFO, "Selected random image: %s", image_to_process_path);
  } else if (args.image_path) {
    image_to_process_path = strdup(args.image_path);
  }

  const char *path = image_to_process_path;
  if (!path) {
    logging(ERROR, "Could not resolve path for %s", image_to_process_path);
    free(image_to_process_path);
    free_config(app_config);
    free_cli_args(&args);
    return 1;
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

  // Palette structure initiallation
  Palette palette = {0};
  palette.wallpaper = path;

  // Apply settings from CLI
  palette.mode = args.mode;
  palette.cols16_mode = args.cols16_mode;
  palette.saturation = args.saturation;
  palette.contrast = args.contrast;
  palette.alpha = args.alpha;

  // Loads colors from cache
  if (load_palette_from_cache(&palette, args.out_dir, args.backend) != 0) {
    RawImage *raw_img = image_load_from_file(path);
    if (!raw_img) {
      logging(ERROR, "Error loading image from file: %s", path);
      free_config(app_config);
      free_cli_args(&args);
      return -1;
    }

    if (process_backend(backend, raw_img, &palette) != 0) {
      logging(ERROR, "Image processing failed!");
      image_free(raw_img);
      free_config(app_config);
      free_cli_args(&args);
      return -1;
    }
    image_free(raw_img);
    process_colors(&palette, args.saturation, args.contrast);
    save_palette_to_cache(&palette, args.out_dir, args.backend);
  }

  // Generates template files
  process_template(args.out_dir, &palette);

  // Reloads the app colors
  apply_colors_to_apps(args.out_dir, args.no_reload);

  // Runs post hooks
  run_hook_script(args.script_path);

  // Updates the config wallpaper path
  free(app_config->current_wallpaper);
  app_config->current_wallpaper = strdup(path);

  // Updates the config file to the current values
  free(app_config->out_dir);
  app_config->out_dir = strdup(args.out_dir);
  app_config->mode = args.mode;
  app_config->cols16_mode = args.cols16_mode;
  free(app_config->backend);
  app_config->backend = strdup(args.backend);

  // Saves the config
  save_config(app_config);

  // Prints the generated color palatte
  preview_palette();

  free_config(app_config);
  free_cli_args(&args);
  return 0;
}
