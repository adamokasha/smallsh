#define _POSIX_C_SOURCE 199309L 

#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void register_ignore_SIGINT() {
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = SIG_IGN;
  sigaction(SIGINT, &sigAction, NULL);
}

void register_restore_SIGINT() {
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = SIG_DFL;
  sigaction(SIGINT, &sigAction, NULL);
}