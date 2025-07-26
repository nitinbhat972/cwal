#include "hooks.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h> 
#include <limits.h>
#include <string.h>

void run_hook_script(char *script_path) {
  if (!script_path) {
    return;
  }

  char *command = malloc(strlen(script_path) + 20);
  if (!command) {
      logging(ERROR, "Failed to allocate memory for command");
      return;
  }
  snprintf(command, strlen(script_path) + 20, "%s > /dev/null 2>&1", script_path);

  logging(INFO, "Running hook script: %s", script_path);
  int result = system(command);
  if (result == -1) {
    logging(ERROR, "Failed to execute hook script.");
  } else if (result != 0) {
    logging(WARN, "Hook script exited with non-zero status: %d", result);
  }
  free(command);
}
