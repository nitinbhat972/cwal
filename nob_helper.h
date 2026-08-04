#pragma once

#include "config.h"
#include "nob.h"

typedef struct {
  Cmd base_cmd;
  Procs procs;
} Build_Ctx;

typedef enum {
  PKGCONF_CFLAGS,
  PKGCONF_LIBS,
} PKGCONF_MODE;

/*
 Append the pkgconf flags from the `list` to `cmd`
 `mode` selects either `--cflags` or `--libs`
*/
static bool pkgconf(Cmd *cmd, PKGCONF_MODE mode, const char *const list[]) {
  if (list[0] == NULL)
    return true;

  const char *output = mode == PKGCONF_CFLAGS ? GENERATED_DIR "/pkgconf_cflags"
                                              : GENERATED_DIR "/pkgconf_libs";

  if (!mkdir_if_not_exists(GENERATED_DIR))
    return false;

  Cmd pkg = {0};
  cmd_append(&pkg, "pkgconf", mode == PKGCONF_CFLAGS ? "--cflags" : "--libs");

  for (size_t i = 0; list[i] != NULL; ++i) {
    cmd_append(&pkg, list[i]);
  }

  if (!cmd_run(&pkg, .stdout_path = output))
    return false;

  String_Builder out = {0};

  if (!read_entire_file(output, &out))
    return false;

  String_View sv = sv_trim(sv_from_parts(out.items, out.count));

  while (sv.count > 0) {
    String_View flag = sv_chop_by_delim(&sv, ' ');
    if (flag.count > 0)
      cmd_append(cmd, temp_sprintf("%.*s", (int)flag.count, flag.data));
  }
  return true;
}

/*
 Append the `CWAL_VERSION` compiler defination from the `VERSION` file at `path`
 to `cmd`
*/
static bool append_version(Cmd *cmd, const char *path) {
  String_Builder version = {0};
  if (!read_entire_file(path, &version))
    return false;

  String_View sv = sv_trim(sv_from_parts(version.items, version.count));

  cmd_append(cmd,
             temp_sprintf("-DCWAL_VERSION=\"%.*s\"", (int)sv.count, sv.items));

  return true;
}

static void append_compiler(Cmd *cmd) {
#ifdef CUSTOM_CC
  cmd_append(cmd, CUSTOM_CC);
#else
  nob_cc(cmd);
#endif

  nob_cc_flags(cmd);
}

static bool get_deps(const char *path, Cmd *out) {
  if (!file_exists(path))
    return false;

  String_Builder dep_content = {0};
  if (!read_entire_file(path, &dep_content))
    return false;

  String_View sv = sv_from_parts(dep_content.items, dep_content.count);
  sv_chop_by_delim(&sv, ':');
  while (sv.count > 0) {
    String_View line = sv_trim(sv_chop_by_delim(&sv, '\n'));
    if (line.count > 0 && line.data[line.count - 1] == '\\')
      line.count--;

    while (line.count > 0) {
      String_View dep = sv_trim(sv_chop_by_delim(&line, ' '));
      if (dep.count > 0)
        cmd_append(out, temp_sprintf("%.*s", (int)dep.count, dep.data));
    }
  }

  return true;
}
