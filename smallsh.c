#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h> // getpid
#include <signal.h>

#include "smallshutils.h"
#include "smallshtok.h"
#include "smallshcd.h"
#include "smallshstatus.h"
#include "smallshfgspawn.h"

void prompt(char **buf, size_t *buflen)
{
  printf(": ");
  getline(buf, buflen, stdin);
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

void execUserCommand(struct UserInput *userInput, struct CommandStatus *commandStatus)
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
    // printf("Executing status\n");
  }
  else
  {
    // printf("Executing other command\n");
    spawnForegroundProcess(userInput, commandStatus);
  }
}

int main()
{
  char *buf = NULL;
  size_t buflen;

  struct CommandStatus *commandStatus = malloc(sizeof(struct CommandStatus));
  
  while (1)
  {
    struct UserInput *userInput = malloc(sizeof(struct UserInput));
    initializeUserInput(userInput);

    prompt(&buf, &buflen);
    fflush(stdout);
    parseUserInput(buf, userInput);

    if (userInput->command != NULL)
    {
      execUserCommand(userInput, commandStatus);
    }

    freeUserInput(userInput);

    free(buf);
    buf = NULL;
  }
}
