#define NOB_IMPLEMENTATION

#include "../nob_helper.h"

static bool walk_dir_cb(Walk_Entry entry) {
  Build_Ctx *ctx = entry.data;
  const char *input = entry.path;

  if (entry.type == FILE_DIRECTORY) {
    if (!mkdir_if_not_exists(temp_sprintf(GENERATED_DIR "/%s", input)))
      return false;
    return true;
  }

  if (entry.type != FILE_REGULAR)
    return true;

  if (!sv_ends_with_cstr(sv_from_cstr(input), ".c"))
    return true;
  if (sv_eq(sv_from_cstr(input), sv_from_cstr(SRC_DIR "/nob.c")))
    return true;

  Cmd cmd = {0};
  nob_cmd_extend(&cmd, &ctx->base_cmd);

  const char *output = temp_sprintf(GENERATED_DIR "/%s.o", input);
  const char *depfile_path = temp_sprintf("%s.d", output);

  Cmd deps = {0};
  int ok;

  if (!get_deps(depfile_path, &deps))
    ok = 1;
  else {
    cmd_append(&deps, "VERSION", "config.h");
    ok = needs_rebuild(output, deps.items, deps.count);
  }

  if (ok < 0)
    return false;
  if (ok == 0)
    return true;

  nob_cc_output(&cmd, output);
  nob_cc_inputs(&cmd, input);
  nob_cmd_append(&cmd, "-MMD", "-MF", depfile_path, "-MT", output);

  if (!cmd_run(&cmd, .async = &ctx->procs, .max_procs = PROCS))
    return false;

  return true;
}

static bool build(Build_Ctx *ctx) {
  append_compiler(&ctx->base_cmd);
  cmd_append(&ctx->base_cmd, "-c");

  for (size_t i = 0; CFLAGS[i] != NULL; ++i)
    cmd_append(&ctx->base_cmd, CFLAGS[i]);

  if (!append_version(&ctx->base_cmd, "VERSION"))
    return false;

  for (size_t i = 0; INCLUDES[i] != NULL; ++i)
    cmd_append(&ctx->base_cmd, temp_sprintf("-I%s", INCLUDES[i]));

  if (!pkgconf(&ctx->base_cmd, PKGCONF_CFLAGS, PKGS))
    return false;

  if (!walk_dir(SRC_DIR, walk_dir_cb, .data = ctx))
    return false;

  if (!procs_flush(&ctx->procs))
    return false;

  return true;
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "nob.h", "nob_helper.h", "config.h");

  Build_Ctx ctx = {0};

  if (!mkdir_if_not_exists(GENERATED_DIR))
    return 1;

  if (!build(&ctx))
    return 1;

  return 0;
}
