#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h> // getpid

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
    // TODO: remove
    printf("Is comment or empty string.\n");
    return;
  }
  buildUserInput(buf, userInput);
}

int main()
{
  char *buf = NULL;
  size_t buflen;
  struct UserInput *userInput = malloc(sizeof(struct UserInput));
  userInput->isBackgroundProcess = false;

  prompt(&buf, &buflen);
  parseUserInput(buf, userInput);
  fflush(stdin);

  free(buf);
  buf = NULL;

  return EXIT_SUCCESS;
}
