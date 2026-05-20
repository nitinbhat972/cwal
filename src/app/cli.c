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
#include "utils/utils.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *prog_name) {
  fprintf(stderr, BOLD "Usage:" RESET " %s [OPTIONS] --img <image_path>\n",
          prog_name);
  fprintf(stderr, BOLD "Options:" RESET "\n");
  fprintf(stderr, "  " YELLOW "-m, --mode" RESET " " CYAN "<dark|light>" RESET
                  "    Set theme mode (overrides config)\n");
  fprintf(stderr, "  " YELLOW "-c, --cols16-mode" RESET " " CYAN
                  "<darken|lighten>" RESET
                  " Set 16-color generation mode (overrides config)\n");
  fprintf(stderr, "  " YELLOW "-s, --saturation" RESET " " CYAN "<float>" RESET
                  "   Set overall saturation (overrides config)\n");
  fprintf(stderr, "  " YELLOW "-C, --contrast" RESET " " CYAN "<float>" RESET
                  "     Set contrast ratio (overrides config)\n");
  fprintf(stderr, "  " YELLOW "-a, --alpha" RESET " " CYAN "<float>" RESET
                  "        Set alpha transparency (0.0-1.0, overrides "
                  "config)\n");
  fprintf(stderr, "  " YELLOW "-o, --out-dir" RESET " " CYAN "<path>" RESET
                  "       Set output directory for all generated files "
                  "(overrides config)\n");
  fprintf(stderr, "  " YELLOW "-b, --backend" RESET " " CYAN "<name>" RESET
                  "       Set image processing backend (overrides config)\n");
  fprintf(stderr, "  " YELLOW "-i, --img" RESET " " CYAN "<image_path>" RESET
                  "     Specify the image path (required)\n");
  fprintf(stderr, "  " YELLOW "-S, --script" RESET " " CYAN
                  "<script_path>" RESET " Run a script after processing\n");
  fprintf(stderr, "  " YELLOW "-n, --no-reload" RESET
                  "            Do not reload applications after processing\n");
  fprintf(stderr, "  " YELLOW "-B, --list-backends" RESET
                  "        List all available image processing backends\n");
  fprintf(stderr, "  " YELLOW "-T, --list-themes" RESET
                  "          List all available themes\n");
  fprintf(stderr,
          "  " YELLOW "-q, --quiet" RESET "                Suppress all "
          "output\n");
  fprintf(stderr, "  " YELLOW "-r, --random" RESET " " CYAN "[directory]" RESET
                  "   Select random image (uses config default if directory "
                  "omitted)\n");
  fprintf(stderr, "  " YELLOW "-t, --theme" RESET " " CYAN
                  "<theme_name|random_dark|random_light|random_all>" RESET
                  " Select a theme or a random one\n");
  fprintf(stderr, "  " YELLOW "-p, --preview" RESET
                  "              show palette preview\n");
  fprintf(stderr, "  " YELLOW "-N, --skip-cursor" RESET
                  "          Skip writing the cursor color sequence\n");
  fprintf(stderr, "  " YELLOW "-v, --version" RESET
                  "              Show the version number\n");
  fprintf(stderr, "  " YELLOW "-h, --help" RESET
                  "                 Display this help message\n");
}

CliStatus parse_cli_args(int argc, char **argv, Config *config, CliArgs *args) {
  args->opts = config->opts; // copy all shared defaults from config
  args->opts.backend =
      strdup(config->opts.backend ? config->opts.backend : "cwal");
  args->opts.script_path =
      config->opts.script_path ? strdup(config->opts.script_path) : NULL;
  args->opts.out_dir = strdup(config->opts.out_dir);
  args->opts.random_dir =
      config->opts.random_dir ? strdup(config->opts.random_dir) : NULL;
  args->image_path = NULL;
  args->backend_specified = false;
  args->no_reload = false;
  args->list_backends = false;
  args->list_themes = false;
  args->quiet = false;
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
      {"skip-cursor", no_argument, 0, 'N'},
      {"version", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt;
  int long_index = 0;
  optind = 1;

  while ((opt = getopt_long(argc, argv, "m:c:s:C:a:b:i:S:o:nBTqr::t:pNvh",
                            long_options, &long_index)) != -1) {
    const char *actual_opt = (optarg && argv[optind - 1] == optarg)
                                 ? argv[optind - 2]
                                 : argv[optind - 1];

    if (opt != '?' && actual_opt && strncmp(actual_opt, "--", 2) == 0) {
      const char *name = long_options[long_index].name;
      size_t name_len = strlen(name);
      if (strncmp(actual_opt + 2, name, name_len) != 0 ||
          (actual_opt[2 + name_len] != '\0' &&
           actual_opt[2 + name_len] != '=')) {
        fprintf(stderr, "%s: unrecognized option '%s'\n", argv[0], actual_opt);
        print_usage(argv[0]);
        return CLI_ERROR;
      }
    }

    switch (opt) {
    case 'm':
      if (strncmp(optarg, "dark", 5) == 0) {
        args->opts.mode = DARK;
      } else if (strncmp(optarg, "light", 6) == 0) {
        args->opts.mode = LIGHT;
      } else {
        logging(ERROR, "Invalid mode: %s. Use 'dark' or 'light'.", optarg);
        return CLI_ERROR;
      }
      break;
    case 'c':
      if (strncmp(optarg, "darken", 7) == 0) {
        args->opts.cols16_mode = DARKEN;
      } else if (strncmp(optarg, "lighten", 8) == 0) {
        args->opts.cols16_mode = LIGHTEN;
      } else {
        logging(ERROR, "Invalid cols16-mode: %s. Use 'darken' or 'lighten'.",
                optarg);
        return CLI_ERROR;
      }
      break;
    case 's':
      args->opts.saturation = atof(optarg);
      break;
    case 'C':
      args->opts.contrast = atof(optarg);
      break;
    case 'a':
      args->opts.alpha = atof(optarg);
      if (args->opts.alpha < 0.0f || args->opts.alpha > 1.0f) {
        logging(ERROR, "Invalid alpha value: %s. Must be between 0.0 and 1.0.",
                optarg);
        return CLI_ERROR;
      }
      break;
    case 'b':
      free(args->opts.backend);
      args->opts.backend = strdup(optarg);
      args->backend_specified = true;
      break;
    case 'i':
      free(args->image_path);
      args->image_path = strdup(optarg);
      break;
    case 'S':
      free(args->opts.script_path);
      args->opts.script_path = strdup(optarg);
      break;
    case 'o':
      free(args->opts.out_dir);
      args->opts.out_dir = strdup(optarg);
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
        free(args->opts.random_dir);
        args->opts.random_dir = strdup(optarg);
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
    case 'N':
      args->opts.skip_cursor = true;
      break;
    case 'v':
      printf("cwal v%s\n", CWAL_VERSION);
      return CLI_EXIT;
    case 'h':
      print_usage(argv[0]);
      return CLI_EXIT;
    case '?':
      print_usage(argv[0]);
      return CLI_ERROR;
    }
  }

  if (!args->image_path && !args->list_backends && !args->list_themes &&
      !args->use_random_dir && !args->preview && !args->theme &&
      !args->use_random_theme) {
    logging(ERROR, "Missing --img <image_path>, --random <directory>, or "
                   "--theme <theme_name> argument.");
    print_usage(argv[0]);
    return CLI_ERROR;
  }

  if (args->use_random_dir &&
      (!args->opts.random_dir || strlen(args->opts.random_dir) == 0)) {
    logging(ERROR, "No random directory specified. Please provide one via "
                   "--random <dir> or set random_dir in your config.");
    print_usage(argv[0]);
    return CLI_ERROR;
  }

  if (args->image_path && args->opts.random_dir && args->use_random_dir) {
    logging(ERROR,
            "Cannot use both --img and --random arguments simultaneously.");
    return CLI_ERROR;
  }

  return CLI_OK;
}

void free_cli_args(CliArgs *args) {
  if (args) {
    free(args->image_path);
    free(args->opts.backend);
    free(args->opts.script_path);
    free(args->opts.out_dir);
    free(args->opts.random_dir);
    free(args->theme);
  }
}
