#pragma once

#include "config.h"
#include "nob.h"

#ifdef GENERATE_COMPILE_COMMANDS
static void cdb_escape(String_Builder *sb, const char *s) {
  for (; *s; ++s) {
    if (*s == '"')
      sb_append_cstr(sb, "\\\"");
    else if (*s == '\\')
      sb_append_cstr(sb, "\\\\");
    else if (*s == '\n')
      sb_append_cstr(sb, "\\n");
    else if (*s == '\r')
      sb_append_cstr(sb, "\\r");
    else if (*s == '\t')
      sb_append_cstr(sb, "\\t");
    else
      da_append(sb, *s);
  }
}
#endif

typedef struct {
  Cmd base_cmd;
  Procs procs;
#ifdef GENERATE_COMPILE_COMMANDS
  String_Builder cdb_buf;
  const char *cdb_dir;
#endif
} Build_Ctx;

#ifdef GENERATE_COMPILE_COMMANDS
static bool cdb_append(Build_Ctx *ctx, const char *input, const char *output,
                       Cmd *cmd) {
  if (!ctx->cdb_dir)
    ctx->cdb_dir = get_current_dir_temp();
  if (!ctx->cdb_dir)
    return false;

  if (ctx->cdb_buf.count > 0)
    sb_append_cstr(&ctx->cdb_buf, ",\n");

  String_Builder render = {0};
  for (size_t i = 0; i < cmd->count; ++i) {
    if (i > 0)
      sb_append_cstr(&render, " ");
    sb_append_cstr(&render, cmd->items[i]);
  }
  sb_append_null(&render);

  const char *abs_input =
      input[0] == '/' ? input : temp_sprintf("%s/%s", ctx->cdb_dir, input);
  const char *abs_output =
      output[0] == '/' ? output : temp_sprintf("%s/%s", ctx->cdb_dir, output);

  sb_append_cstr(&ctx->cdb_buf, "  {\n    \"directory\": \"");
  cdb_escape(&ctx->cdb_buf, ctx->cdb_dir);
  sb_append_cstr(&ctx->cdb_buf, "\",\n    \"file\": \"");
  cdb_escape(&ctx->cdb_buf, abs_input);
  sb_append_cstr(&ctx->cdb_buf, "\",\n    \"output\": \"");
  cdb_escape(&ctx->cdb_buf, abs_output);
  sb_append_cstr(&ctx->cdb_buf, "\",\n    \"command\": \"");
  cdb_escape(&ctx->cdb_buf, render.items);
  sb_append_cstr(&ctx->cdb_buf, "\"\n  }");
  free(render.items);
  return true;
}

static bool cdb_write(Build_Ctx *ctx) {
  if (!mkdir_if_not_exists(BUILD_DIR))
    return false;
  String_Builder out = {0};
  sb_append_cstr(&out, "[\n");
  sb_append_buf(&out, ctx->cdb_buf.items, ctx->cdb_buf.count);
  sb_append_cstr(&out, "\n]\n");
  sb_append_null(&out);
  bool ok = write_entire_file(BUILD_DIR "/compile_commands.json", out.items,
                              out.count - 1);
  free(out.items);
  return ok;
}
#endif

typedef enum {
  PKGCONF_CFLAGS,
  PKGCONF_LIBS,
} Pkgconf_Mode;

typedef enum {
  DEBUG,
  RELEASE,
} Build_Type;

/*
 Append the pkgconf flags from the `list` to `cmd`
 `mode` selects either `--cflags` or `--libs`
*/
static bool pkgconf(Cmd *cmd, Pkgconf_Mode mode, const char *const list[]) {
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

  if (BUILD_TYPE == RELEASE) {
    cmd_append(cmd, "-O3", "-DNDEBUG");
  } else {
    cmd_append(cmd, "-g");
  }
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

static const char *get_install_dir() {
  const char *destdir = getenv("DESTDIR");

  String_Builder path = {0};

  if (destdir)
    sb_append_cstr(&path, destdir);

  const char *base = INSTALL_DIR;

  if (base[0] == '~' && (base[1] == '\0' || base[1] == '/')) {
    sb_append_cstr(&path, getenv("HOME"));
    sb_append_cstr(&path, base + 1);
  } else {
    sb_append_cstr(&path, base);
  }

  sb_append_null(&path);
  return temp_strndup(path.items, path.count);
}

static bool mkdir_p(const char *src) {
  String_View parent = sv_from_cstr(src);
  String_Builder dir_sb = {0};
  if (parent.count > 0 && parent.data[0] == '/')
    sb_append_cstr(&dir_sb, "/");

  while (parent.count > 0) {
    String_View part = sv_chop_by_delim(&parent, '/');
    if (part.count == 0)
      continue;

    if (dir_sb.count > 0 && dir_sb.items[dir_sb.count - 1] != '/')
      sb_append_cstr(&dir_sb, "/");

    sb_append_sv(&dir_sb, part);

    char *path = temp_strndup(dir_sb.items, dir_sb.count);

    if (!mkdir_if_not_exists(path))
      return false;
  }

  return true;
}

static bool copy_recursively(const char *src, const char *dest) {
  File_Type type = get_file_type(src);
  if (type < 0)
    return false;

  if (!mkdir_p(temp_dir_name(dest)))
    return false;

  switch (type) {
  case FILE_REGULAR:
    return copy_file(src, dest);
  case FILE_DIRECTORY:
    return copy_directory_recursively(src, dest);
  default:
    nob_log(ERROR, "Cannot install %s: unsupported filetype", src);
    return false;
  }

  return true;
}
