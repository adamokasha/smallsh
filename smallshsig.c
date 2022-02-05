#define _POSIX_C_SOURCE 200809L 

#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "smallshsig.h"

int fgOnlyMode = 0;

void register_ignore_SIGINT() {
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = SIG_IGN;
  sigAction.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sigAction, NULL);
}

void register_restore_SIGINT() {
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = SIG_DFL;
  sigAction.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sigAction, NULL);
}

void register_ignore_SIGTSTP() {
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = SIG_IGN;
  sigAction.sa_flags = SA_RESTART;
  sigaction(SIGTSTP, &sigAction, NULL);
}

void handle_SIGTSTP(int signo) {
  char *activatingMessage = "\nEntering foreground-only mode (& is not ignored)\n";
  char *deactivatingMessage = "\nExiting foreground-only mode\n";
  char *currentMessage;
  int strLen;

  if (fgOnlyMode == 0) {
    currentMessage = activatingMessage;
    fgOnlyMode = 1;
    strLen = 50;
  } else {
    currentMessage = deactivatingMessage;
    fgOnlyMode = 0;
    strLen = 30;
  }

  write(STDOUT_FILENO, currentMessage, strLen - 1);
  currentMessage = NULL;
}

void register_toggle_fg_mode() {
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = handle_SIGTSTP;
  sigfillset(&sigAction.sa_mask);

  sigaction(SIGTSTP, &sigAction, NULL);
  fflush(stdout);
}