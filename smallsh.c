#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct UserInput
{
  char *command;
  char args[512];
  char *inputRedirect;
  char *outputRedirect;
  char *inputFile;
  char *outputFile;
  bool isBackgroundProcess;
};

/*
  Removes a trailing '\n' from character string
*/
void cleanTrailingNewlineFromString(char *str)
{
  char *newLinePtr = str + (strlen(str) - 1);

  if (*newLinePtr == '\n')
  {
    *newLinePtr = '\0';
  }

  newLinePtr = NULL;
}

void prompt(char **buf, size_t *buflen)
{
  printf(": ");
  getline(buf, buflen, stdin);
}

bool isComment(char *buf)
{
  if (buf[0] == '#')
    return true;
  return false;
}

bool isEmptyString(char *buf)
{
  char *bufCopy = calloc(strlen(buf) + 1, sizeof(char));
  strcpy(bufCopy, buf);
  char *token = NULL;
  char *savePtr = NULL;

  token = strtok_r(bufCopy, " ", &savePtr);

  if (token == NULL) {
    free(bufCopy);
    bufCopy = NULL;
    // TODO: remove
    printf("TOKEN IS NULL\n");
    return true;
  }

  free(bufCopy);
  bufCopy = NULL;

  return false;
}

void parseUserInput(char *buf)
{
  cleanTrailingNewlineFromString(buf);

  if (isComment(buf) || isEmptyString(buf))
  {
    // TODO: remove
    printf("Is comment or empty string.\n");
    return;
  }
}

int main()
{
  char *buf = NULL;
  size_t buflen;

  prompt(&buf, &buflen);
  parseUserInput(buf);
  fflush(stdin);

  free(buf);
  buf = NULL;

  return EXIT_SUCCESS;
}
