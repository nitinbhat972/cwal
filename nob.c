/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "config.h"
#include <stdio.h>
#include <string.h>
#define NOB_IMPLEMENTATION

#include "nob_helper.h"

static bool link_cb(Walk_Entry entry) {
  Cmd *cmd = entry.data;
  const char *input = entry.path;

  if (entry.type != FILE_REGULAR)
    return true;

  if (!sv_ends_with_cstr(sv_from_cstr(input), ".o"))
    return true;

#ifndef USE_LIBIMAGEQUANT
  if (sv_ends_with_cstr(sv_from_cstr(input), "libimagequant.c.o"))
    return true;
#endif
#ifndef USE_LUA_BACKEND
  if (sv_ends_with_cstr(sv_from_cstr(input), "lua_backend.c.o"))
    return true;
#endif

  cmd_append(cmd, temp_strdup(entry.path));

  return true;
}

static bool link_files() {
  Cmd cmd = {0};

  append_compiler(&cmd);

  if (BUILD_TYPE == RELEASE)
    cmd_append(&cmd, "-s");

  for (size_t i = 0; CFLAGS[i] != NULL; ++i)
    cmd_append(&cmd, CFLAGS[i]);

  if (!walk_dir(GENERATED_DIR, link_cb, .data = &cmd))
    return 1;

  nob_cc_output(&cmd, BUILD_DIR "/cwal");

  if (!pkgconf(&cmd, PKGCONF_LIBS, PKGS))
    return false;

  for (size_t i = 0; LIBS[i] != NULL; ++i)
    cmd_append(&cmd, temp_sprintf("-l%s", LIBS[i]));

  if (!cmd_run(&cmd))
    return false;

  return true;
}

static bool bootstrap() {
  if (!mkdir_if_not_exists(BUILD_DIR))
    return 1;

  Cmd cmd = {0};

  const char *output = BUILD_DIR "/nob_bootstrap";
  const char *input = SRC_DIR "/nob.c";

  if (!file_exists(output)) {
    append_compiler(&cmd);
    nob_cc_output(&cmd, output);
    nob_cc_inputs(&cmd, input);

    if (!cmd_run(&cmd))
      return false;
  }

  nob_cmd_append(&cmd, BUILD_DIR "/nob_bootstrap");

  if (!cmd_run(&cmd))
    return false;

  return true;
}

static bool build() {
  if (!bootstrap())
    return false;

  if (!link_files())
    return false;

  return true;
}

static bool clean_cb(Walk_Entry entry) {
  if (entry.type == FILE_DIRECTORY)
    return true;

  if (!delete_file(entry.path))
    return false;

  return true;
}

static bool clean() {
  if (!file_exists(BUILD_DIR))
    return true;

  if (!walk_dir(BUILD_DIR, clean_cb))
    return false;

  return true;
}

static bool install() {
  const char *install_dir = get_install_dir();

  for (size_t i = 0; INSTALL_FILES[i].src != NULL; ++i) {
    const char *src = INSTALL_FILES[i].src;
    const char *dest =
        temp_sprintf("%s/%s", install_dir, INSTALL_FILES[i].dest);

    if (!copy_recursively(src, dest))
      return false;
  }
  return true;
}

static bool uninstall_cb(Walk_Entry entry) {
  if (!delete_file(entry.path))
    return false;

  return true;
}

static bool uninstall() {
  const char *install_dir = get_install_dir();

  for (size_t i = 0; INSTALL_FILES[i].dest != NULL; ++i) {
    const char *dest =
        temp_sprintf("%s/%s", install_dir, INSTALL_FILES[i].dest);

    if (!file_exists(dest))
      continue;

    File_Type type = get_file_type(dest);
    if (type < 0) {
      return false;
    }

    if (type == FILE_DIRECTORY) {
      if (!walk_dir(dest, uninstall_cb, .post_order = true))
        return false;
    } else {
      if (!delete_file(dest))
        return false;
    }
  }

  return true;
}

static void print_usage(const char *program_name) {
  fprintf(stderr, "Usage: %s <Command>\n\n", program_name);
  fprintf(stderr, "Commands:\n");
  fprintf(stderr, "  build\t\tBuild project at BUILD_DIR\n");
  fprintf(stderr, "  clean\t\tRemove all files in BUILD_DIR\n");
  fprintf(stderr, "  install\tInstall cwal and artifacts in INSTALL_DIR"
                   " (optionally prefixed by DESTDIR)\n");
  fprintf(stderr, "  uninstall\tUninstall cwal and all artifacts\n");
  fprintf(stderr, "  help\t\tPrint this help message\n\n");

  fprintf(stderr, "NOTE:\n");
  fprintf(stderr, "  This nob build is specific to cwal and may not work"
                   " in your project.\n");
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "nob.h", "nob_helper.h", "config.h");

  const char *program_name = shift_args(&argc, &argv);

  if (argc > 0) {
    const char *subcmd = shift_args(&argc, &argv);
    if (strcmp(subcmd, "build") == 0) {
      if (!build())
        return 1;
    } else if (strcmp(subcmd, "clean") == 0) {
      if (!clean())
        return 1;
    } else if (strcmp(subcmd, "install") == 0) {
      if (!build())
        return 1;
      if (!install())
        return 1;
    } else if (strcmp(subcmd, "uninstall") == 0) {
      if (!uninstall())
        return 1;
    } else if (strcmp(subcmd, "help") == 0) {
      print_usage(program_name);
      return 0;
    } else {
      nob_log(ERROR, "Unknown subcommand: %s", subcmd);
      print_usage(program_name);
      return 1;
    }
  } else {
    if (!build())
      return 1;
  }

  return 0;
}
