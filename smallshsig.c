/*
  This file contains code for registering sig actions and their
  handlers where needed
*/
/*
  Citation
  Solution for compiler error with sigaction struct "has initializer but incomplete type"
  Citation: https://edstem.org/us/courses/16718/discussion/1062108
*/
#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "smallshsig.h"

int fgOnlyMode = 0;

/*
  Registers an sig_action to ignore SIGINT
*/
void register_ignore_SIGINT()
{
  /*
    Citation
    Solution for compiler warnning "missing braces around initializer"
    Source URL: https://stackoverflow.com/questions/13746033/how-to-repair-warning-missing-braces-around-initializer
  */
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = SIG_IGN;
  sigAction.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sigAction, NULL);
}

/*
  Restores the default behaviour of SIGINT
  by registering a sig action
*/
void register_restore_SIGINT()
{
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = SIG_DFL;
  sigAction.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sigAction, NULL);
}

/*
  Registers a sig action to ignore SIGTSTP
*/
void register_ignore_SIGTSTP()
{
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = SIG_IGN;
  sigAction.sa_flags = SA_RESTART;
  sigaction(SIGTSTP, &sigAction, NULL);
}

/*
  This sig action handler is used for toggling
  foreground-only mode in the application. Prints
  a message to the terminal to inform the user if
  foreground-only mode is active or inactive.

  It relies on a global variable `fgOnlyMode`.
  When it is set to 0, foreground only mode
  is inactive, and when set 1 to, it is active.
*/
void handle_SIGTSTP(int signo)
{
  char *activatingMessage = "\nEntering foreground-only mode (& is now ignored)\n";
  char *deactivatingMessage = "\nExiting foreground-only mode\n";
  char *currentMessage;
  int strLen;

  if (fgOnlyMode == 0)
  {
    currentMessage = activatingMessage;
    fgOnlyMode = 1;
    strLen = 50;
  }
  else
  {
    currentMessage = deactivatingMessage;
    fgOnlyMode = 0;
    strLen = 30;
  }

  write(STDOUT_FILENO, currentMessage, strLen - 1);
  currentMessage = NULL;
}

/*
  Registers handle_SIGTSTP to a sig action
  to provide toggling between foreground-only
  on/off.
*/
void register_toggle_fg_mode()
{
  struct sigaction sigAction = {{0}};
  sigAction.sa_handler = handle_SIGTSTP;
  sigfillset(&sigAction.sa_mask);

  sigaction(SIGTSTP, &sigAction, NULL);
  fflush(stdout);
}