/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "cli.h"
#include "version.h"
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
          "  -m, --mode <dark|light>    Set theme mode (overrides config)\n");
  fprintf(stderr, "  -c, --cols16-mode <darken|lighten> Set 16-color generation "
                  "mode (overrides config)\n");
  fprintf(
      stderr,
      "  -s, --saturation <float>   Set overall saturation (overrides config)\n");
  fprintf(stderr,
          "  -C, --contrast <float>     Set contrast ratio (overrides config)\n");
  fprintf(stderr, "  -a, --alpha <float>        Set alpha transparency (0.0-1.0, "
                  "overrides config)\n");
  fprintf(stderr, "  -o, --out-dir <path>       Set output directory for all "
                  "generated files (overrides config)\n");
  fprintf(stderr, "  -b, --backend <name>       Set image processing backend "
                  "(overrides config)\n");
  fprintf(stderr,
          "  -i, --img <image_path>     Specify the image path (required)\n");
  fprintf(stderr, "  -S, --script <script_path> Run a script after processing\n");
  fprintf(
      stderr,
      "  -n, --no-reload            Do not reload applications after processing\n");
  fprintf(stderr, "  -B, --list-backends        List all available image "
                  "processing backends\n");
  fprintf(stderr, "  -T, --list-themes          List all available themes\n");
  fprintf(stderr, "  -q, --quiet                Suppress all output\n");
  fprintf(stderr, "  -r, --random [directory]   Select random image (uses config default if directory omitted)\n");
  fprintf(stderr, "  -t, --theme <theme_name|random_dark|random_light|random_all> "
                  "Select a theme or a random one\n");
  fprintf(stderr, "  -p, --preview              show palette preview\n");
  fprintf(stderr, "  -v, --version              Show the version number\n");
  fprintf(stderr, "  -h, --help                 Display this help message\n");
}

int parse_cli_args(int argc, char **argv, Config *config, CliArgs *args) {
  args->image_path = NULL;
  args->mode = config->mode;
  args->cols16_mode = config->cols16_mode;
  args->saturation = config->saturation;
  args->contrast = config->contrast;
  args->alpha = config->alpha;
  args->backend = strdup(config->backend ? config->backend : "cwal");
  args->script_path = config->script_path ? strdup(config->script_path) : NULL;
  args->out_dir = strdup(config->out_dir);
  args->no_reload = false;
  args->list_backends = false;
  args->list_themes = false;
  args->quiet = false;
  args->random_dir = config->random_dir ? strdup(config->random_dir) : NULL;
  args->use_random_dir = false;
  args->use_random_theme = false;
  args->random_mode = RANDOM_ALL;
  args->theme = NULL;
  args->preview = false;

  static struct option long_options[] = {
      {"mode", required_argument, 0, 'm'},
      {"cols16-mode", required_argument, 0, 'c'},
      {"saturation", required_argument, 0, 's'},
      {"contrast", required_argument, 0, 'C'},
      {"alpha", required_argument, 0, 'a'},
      {"backend", required_argument, 0, 'b'},
      {"img", required_argument, 0, 'i'},
      {"script", required_argument, 0, 'S'},
      {"out-dir", required_argument, 0, 'o'},
      {"no-reload", no_argument, 0, 'n'},
      {"list-backends", no_argument, 0, 'B'},
      {"list-themes", no_argument, 0, 'T'},
      {"quiet", no_argument, 0, 'q'},
      {"random", optional_argument, 0, 'r'},
      {"theme", required_argument, 0, 't'},
      {"preview", no_argument, 0, 'p'},
      {"version", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt;
  int long_index = 0;
  optind = 1;

  while ((opt = getopt_long(argc, argv, "m:c:s:C:a:b:i:S:o:nBTqr::t:pvh",
                            long_options, &long_index)) != -1) {
    const char *actual_opt = (optarg && argv[optind - 1] == optarg)
                                 ? argv[optind - 2]
                                 : argv[optind - 1];

    if (opt != '?' && actual_opt && strncmp(actual_opt, "--", 2) == 0) {
      const char *name = long_options[long_index].name;
      size_t name_len = strlen(name);
      if (strncmp(actual_opt + 2, name, name_len) != 0 ||
          (actual_opt[2 + name_len] != '\0' && actual_opt[2 + name_len] != '=')) {
        logging(ERROR, "Unknown option: %s", actual_opt);
        return 1;
      }
    }

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
    case 'C':
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
    case 'S':
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
    case 'B':
      args->list_backends = true;
      break;
    case 'T':
      args->list_themes = true;
      break;
    case 'q':
      args->quiet = true;
      break;
    case 'r':
      if (!optarg && optind < argc && argv[optind][0] != '-') {
        optarg = argv[optind++];
      }

      if (optarg) {
        free(args->random_dir);
        args->random_dir = normalize_cli_path(optarg);
      }
      args->use_random_dir = true;
      break;
    case 't':
      if (strncmp(optarg, "random_dark", 12) == 0) {
        args->random_mode = RANDOM_DARK;
        args->use_random_theme = true;
      } else if (strncmp(optarg, "random_light", 13) == 0) {
        args->random_mode = RANDOM_LIGHT;
        args->use_random_theme = true;
      } else if (strncmp(optarg, "random_all", 11) == 0) {
        args->random_mode = RANDOM_ALL;
        args->use_random_theme = true;
      } else {
        free(args->theme);
        args->theme = strdup(optarg);
        args->use_random_theme = false;
      }
      break;
    case 'p':
      args->preview = true;
      break;
    case 'v':
      printf("version %s\n", CWAL_VERSION);
      return 1;
    case 'h':
      print_usage(argv[0]);
      return 1;
    case '?':
      print_usage(argv[0]);
      return 1;
    }
  }

  if (!args->image_path && !args->list_backends && !args->list_themes &&
      !args->use_random_dir && !args->preview && !args->theme &&
      !args->use_random_theme) {
    logging(ERROR, "Missing --img <image_path>, --random <directory>, or "
                   "--theme <theme_name> argument.");
    print_usage(argv[0]);
    return 1;
  }

  if (args->use_random_dir &&
      (!args->random_dir || strlen(args->random_dir) == 0)) {
    logging(ERROR, "No random directory specified. Please provide one via "
                   "--random <dir> or set random_dir in your config.");
    print_usage(argv[0]);
    return 1;
  }

  if (args->image_path && args->random_dir && args->use_random_dir) {
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
