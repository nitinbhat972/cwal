#include "hooks.h"
#include "utils/utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void run_hook_script(char *script_path) {
  if (!script_path)
    return;

  pid_t pid = fork();

  if (pid == -1) {
    logging(ERROR, "Failed to fork process for hook script: %s", script_path);
    return;
  }

  if (pid == 0) {
    // --- Child Process ---
    // Redirect stdout and stderr to /dev/null
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    char *args[] = {script_path, NULL};
    execvp(script_path, args);

    // If execvp gets here, it failed. Check errno.
    if (errno == EACCES) {
      _exit(126); // Not executable
    } else {
      _exit(127); // Not found
    }
  } else {
    // --- Parent Process ---
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      int exit_code = WEXITSTATUS(status);
      if (exit_code == 0) {
        logging(INFO, "Hook script '%s' ran successfully.", script_path);
      } else if (exit_code == 126) {
        logging(WARN, "Hook script '%s' is not executable.", script_path);
      } else if (exit_code == 127) {
        logging(WARN, "Hook script '%s' not found.", script_path);
      } else {
        logging(WARN, "Hook script '%s' exited with non-zero status: %d",
                script_path, exit_code);
      }
    } else {
      logging(ERROR, "Hook script '%s' terminated abnormally.", script_path);
    }
  }
}
