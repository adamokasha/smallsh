#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct UserInput
{
  char *command;
  char *args[512];
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

  if (token == NULL)
  {
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

bool isArgument(char *str)
{
  if (str == NULL)
    return false;
  return (strcmp(str, "<") != 0) && (strcmp(str, ">") != 0) && (strcmp(str, "&") != 0);
}

void buildUserInput(char *buf, struct UserInput *userInput)
{
  char *bufCopy = calloc(strlen(buf) + 1, sizeof(char));
  strcpy(bufCopy, buf);
  char *token = NULL;
  char *savePtr = NULL;

  token = strtok_r(bufCopy, " ", &savePtr);

  userInput->command = calloc(strlen(token) + 1, sizeof(char));
  strcpy(userInput->command, token);

  token = strtok_r(NULL, " ", &savePtr);
  int i = 0;

  while (isArgument(token))
  {
    userInput->args[i] = calloc(strlen(token + 1), sizeof(char));
    strcpy(userInput->args[i], token);
    token = strtok_r(NULL, " ", &savePtr);
    i++;
  }
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

  prompt(&buf, &buflen);
  parseUserInput(buf, userInput);
  fflush(stdin);

  free(buf);
  buf = NULL;

  return EXIT_SUCCESS;
}
