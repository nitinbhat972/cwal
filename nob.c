#define NOB_IMPLEMENTATION

#include "nob_helper.h"

static bool walk_dir_cb(Walk_Entry entry) {
  Cmd *cmd = entry.data;
  const char *input = entry.path;

  if (entry.type != FILE_REGULAR)
    return true;

  if (!sv_ends_with_cstr(sv_from_cstr(input), ".o"))
    return true;

  cmd_append(cmd, temp_strdup(entry.path));

  return true;
}

static bool build() {
  Cmd cmd = {0};

  append_compiler(&cmd);
  if (!walk_dir(GENERATED_DIR, walk_dir_cb, .data = &cmd))
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
    return false;

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

  nob_cmd_append(&cmd, output);

  if (!cmd_run(&cmd))
    return false;

  return true;
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "nob.h", "nob_helper.h", "config.h");

  if (!bootstrap())
    return 1;

  if (!build())
    return 1;

  return 0;
}
