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

void execUserCommand(struct UserInput *userInput)
{
  if (strcmp(userInput->command, "exit") == 0)
  {
    // TODO: kill each process individually?
    kill(0, SIGKILL);
  }
  else if (strcmp(userInput->command, "cd") == 0)
  {
    printf("Executing cd\n");
  }
  else if (strcmp(userInput->command, "status") == 0)
  {
    printf("Executing status\n");
  }
  else
  {
    printf("Executing other command\n");
  }
}

int main()
{
  char *buf = NULL;
  size_t buflen;

  while (1)
  {
    struct UserInput *userInput = malloc(sizeof(struct UserInput));
    userInput->isBackgroundProcess = false;

    prompt(&buf, &buflen);
    parseUserInput(buf, userInput);
    fflush(stdin);

    if (userInput->command != NULL)
    {
      execUserCommand(userInput);
    }

    free(buf);
    buf = NULL;

    freeUserInput(userInput);
  }
}
