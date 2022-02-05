#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include "smallshutils.h"
#include "smallshtok.h"
#include "smallshcd.h"
#include "smallshstatus.h"
#include "smallshfgspawn.h"
#include "smallshbgspawn.h"
#include "smallshsig.h"

/*
  Prompts user to enter a command
*/
int prompt(char **buf, size_t *buflen)
{
  printf(": ");
  fflush(stdout);
  return getline(buf, buflen, stdin);
}

/*
  Uses the data from UserInput struct to send the user command
  to either:
  - Built in commands: exit, cd & status
  - Background child process (with & symbol or fgOnlyMode enabled)
  - Foreground child process
*/
void execUserCommand(struct UserInput *userInput, struct CommandStatus *commandStatus, pid_t *spawnPids)
{
  if (strcmp(userInput->command, "exit") == 0)
  {
    // TODO: kill each process individually?
    kill(0, SIGKILL);
  }
  else if (strcmp(userInput->command, "cd") == 0)
  {
    setCommandStatus(commandStatus, BUILTIN, 0);
    smallshcd(userInput->args[1]);
  }
  else if (strcmp(userInput->command, "status") == 0)
  {
    printCommandStatus(commandStatus);
  }
  else
  {
    if (userInput->isBackgroundProcess && fgOnlyMode != 1)
    {
      spawnBackgroundProcess(userInput, commandStatus, spawnPids);
    }
    else
    {
      spawnForegroundProcess(userInput, commandStatus);
    }
  }
}

int main()
{
  char *buf = NULL;
  size_t buflen;
  struct CommandStatus *commandStatus = malloc(sizeof(struct CommandStatus));
  int spawnPids[100] = {0};
  int inputResponse;

  register_ignore_SIGINT();
  register_ignore_SIGTSTP();
  register_toggle_fg_mode();

  while (1)
  {
    struct UserInput *userInput = malloc(sizeof(struct UserInput));
    initializeUserInput(userInput);

    /*
      Citation
      Adapted from comment thread. Used suggested solution by Philip Peiffer.
      The signal that affects this particular block of code is the SIGTSTP
      which is using a custom handler. The handler interrupts this getLine
      in `prompt` and returns error (-1). This loop catches the error, clears
      it and flushes stdout. This avoid the issue discussed in the thread,
      of the foreground-only toggled message having either an extra newline
      or in the case here, the prompt indicator ": " not appearing (but still
      allowing input)
      Source URL: https://edstem.org/us/courses/16718/discussion/1082723
    */
    while ((inputResponse = prompt(&buf, &buflen)) == -1)
    {
      clearerr(stdin);
      printf("\n");
      fflush(stdout);
    }

    // Build the UserInput struct which contains command data
    buildUserInput(buf, userInput);

    if (userInput->command != NULL)
    {
      execUserCommand(userInput, commandStatus, spawnPids);
    }

    printSpawnStatus(spawnPids);
    freeUserInput(userInput);

    free(buf);
    buf = NULL;
  }
}
