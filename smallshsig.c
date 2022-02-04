#define _POSIX_C_SOURCE 199309L 

#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void ignore_SIGINT(struct sigaction sigAction) {
  sigAction.sa_handler = SIG_IGN;
  sigaction(SIGINT, &sigAction, NULL);
}

void use_SIG_DFL(struct sigaction sigAction) {
  sigAction.sa_handler = SIG_DFL;
  sigaction(SIGINT, &sigAction, NULL);
}
