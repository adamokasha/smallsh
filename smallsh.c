#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h> // getpid
#include <signal.h>
#include <sys/types.h>

#include "smallshutils.h"
#include "smallshtok.h"
#include "smallshcd.h"
#include "smallshstatus.h"
#include "smallshfgspawn.h"
#include "smallshbgspawn.h"
#include "smallshsig.h"


int prompt(char **buf, size_t *buflen)
{
  printf(": ");
  fflush(stdout);
  return getline(buf, buflen, stdin);
}

void parseUserInput(char *buf, struct UserInput *userInput)
{
  cleanTrailingNewlineFromString(buf);

  if (isComment(buf) || isEmptyString(buf))
  {
    return;
  }
  buildUserInput(buf, userInput);
}

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

    while ((inputResponse = prompt(&buf, &buflen)) == -1) {
      clearerr(stdin);
      printf("\n");
      fflush(stdout);
    }

    parseUserInput(buf, userInput);

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
