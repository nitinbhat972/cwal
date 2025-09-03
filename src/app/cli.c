#include "cli.h"
#include "utils/path.h"
#include "utils/utils.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *prog_name) {
  fprintf(stderr, "Usage: %s [OPTIONS] --img <image_path>\n", prog_name);
  fprintf(stderr, "Options:\n");
  fprintf(stderr,
          "  --mode <dark|light>    Set theme mode (overrides config)\n");
  fprintf(stderr, "  --cols16-mode <darken|lighten> Set 16-color generation "
                  "mode (overrides config)\n");
  fprintf(
      stderr,
      "  --saturation <float>   Set overall saturation (overrides config)\n");
  fprintf(stderr,
          "  --contrast <float>     Set contrast ratio (overrides config)\n");
  fprintf(stderr, "  --alpha <float>        Set alpha transparency (0.0-1.0, "
                  "overrides config)\n");
  fprintf(stderr, "  --out-dir <path>       Set output directory for all "
                  "generated files (overrides config)\n");
  fprintf(stderr, "  --backend <name>       Set image processing backend "
                  "(overrides config)\n");
  fprintf(stderr,
          "  --img <image_path>     Specify the image path (required)\n");
  fprintf(stderr, "  --script <script_path> Run a script after processing\n");
  fprintf(
      stderr,
      "  --no-reload            Do not reload applications after processing\n");
  fprintf(stderr, "  --list-backends        List all available image "
                  "processing backends\n");
  fprintf(stderr, "  --list-themes          List all available themes\n");
  fprintf(stderr, "  --quiet                Suppress all output\n");
  fprintf(stderr, "  --theme <theme_name|random_dark|random_light|random_all> "
                  "Select a theme or a random one\n");
  fprintf(stderr, "  --preview              show palette preview\n");
  fprintf(stderr, "  --help                 Display this help message\n");
}

int parse_cli_args(int argc, char **argv, Config *config, CliArgs *args) {
  args->image_path = NULL;        // Will be set by --img
  args->mode = DARK;              // Default to DARK mode
  args->cols16_mode = DARKEN;     // Default to DARKEN for 16-color generation
  args->saturation = 0.0f;        // Default saturation
  args->contrast = 1.0f;          // Default contrast
  args->alpha = 1.0f;             // Default alpha
  args->backend = strdup("cwal"); // Default backend
  args->script_path = NULL;       // Will be set by --script
  args->out_dir = strdup(config->out_dir); // Only out_dir is loaded from config
  args->no_reload = false;
  args->list_backends = false;
  args->list_themes = false;
  args->quiet = false;
  args->random_dir = NULL;
  args->random_mode = RANDOM_NONE;
  args->theme = NULL;
  args->preview = false;

  static struct option long_options[] = {
      {"mode", required_argument, 0, 'm'},
      {"cols16-mode", required_argument, 0, 'c'},
      {"saturation", required_argument, 0, 's'},
      {"contrast", required_argument, 0, 't'},
      {"alpha", required_argument, 0, 'a'},
      {"backend", required_argument, 0, 'b'},
      {"img", required_argument, 0, 'i'},
      {"script", required_argument, 0, 'p'},
      {"out-dir", required_argument, 0, 'o'},
      {"no-reload", no_argument, 0, 'n'},
      {"list-backends", no_argument, 0, 'l'},
      {"list-themes", no_argument, 0, 'd'},
      {"quiet", no_argument, 0, 'q'},
      {"random", required_argument, 0, 'r'},
      {"theme", required_argument, 0, 'e'},
      {"preview", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt;
  int long_index = 0;
  optind = 1;

  while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1) {
    switch (opt) {
     case 'm':
       if (strncmp(optarg, "dark", 5) == 0) {
         args->mode = DARK;
       } else if (strncmp(optarg, "light", 6) == 0) {
         args->mode = LIGHT;
       } else {
         logging(ERROR, "Invalid mode: %s. Use 'dark' or 'light'.", optarg);
         return 1;
       }
       break;
     case 'c':
       if (strncmp(optarg, "darken", 7) == 0) {
         args->cols16_mode = DARKEN;
       } else if (strncmp(optarg, "lighten", 8) == 0) {
         args->cols16_mode = LIGHTEN;
       } else {
         logging(ERROR, "Invalid cols16-mode: %s. Use 'darken' or 'lighten'.",
                 optarg);
        return 1;
      }
      break;
    case 's':
      args->saturation = atof(optarg);
      break;
    case 't':
      args->contrast = atof(optarg);
      break;
    case 'a':
      args->alpha = atof(optarg);
      if (args->alpha < 0.0f || args->alpha > 1.0f) {
        logging(ERROR, "Invalid alpha value: %s. Must be between 0.0 and 1.0.",
                optarg);
        return 1;
      }
      break;
    case 'b':
      free(args->backend);
      args->backend = strdup(optarg);
      break;
    case 'i':
      free(args->image_path);
      args->image_path = normalize_cli_path(optarg);
      break;
    case 'p':
      free(args->script_path);
      args->script_path = normalize_cli_path(optarg);
      break;
    case 'o':
      free(args->out_dir);
      args->out_dir = normalize_cli_path(optarg);
      break;
    case 'n':
      args->no_reload = true;
      break;
    case 'l':
      args->list_backends = true;
      break;
    case 'd':
      args->list_themes = true;
      break;
    case 'q':
      args->quiet = true;
      break;
    case 'r':
      free(args->random_dir);
      args->random_dir = normalize_cli_path(optarg);
      break;
     case 'e':
       if (strncmp(optarg, "random_dark", 12) == 0) {
         args->random_mode = RANDOM_DARK;
       } else if (strncmp(optarg, "random_light", 13) == 0) {
         args->random_mode = RANDOM_LIGHT;
       } else if (strncmp(optarg, "random_all", 11) == 0) {
         args->random_mode = RANDOM_ALL;
       } else {
        free(args->theme);
        args->theme = strdup(optarg);
      }
      break;
    case 'v':
      args->preview = true;
      break;
    case 'h':
      print_usage(argv[0]);
      return 1;
    case '?':
      print_usage(argv[0]);
      return 1;
    }
  }

  if (!args->image_path && !args->list_backends && !args->list_themes &&
      !args->random_dir && !args->preview && !args->theme &&
      args->random_mode == RANDOM_NONE) {
    logging(ERROR, "Missing --img <image_path>, --random <directory>, or "
                   "--theme <theme_name> argument.");
    print_usage(argv[0]);
    return 1;
  }

  if (args->image_path && args->random_dir) {
    logging(ERROR,
            "Cannot use both --img and --random arguments simultaneously.");
    return 1;
  }

  return 0;
}

void free_cli_args(CliArgs *args) {
  if (args) {
    free(args->image_path);
    free(args->backend);
    free(args->script_path);
    free(args->out_dir);
    free(args->theme);
  }
}
