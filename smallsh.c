#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h> // getpid

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

char *handleExpansion(char *str)
{
  char *strCopy = calloc(strlen(str) + 1, sizeof(char));
  strCopy = strcpy(strCopy, str);
  char *strCopyPtr = strCopy;
  char *expanded = calloc(strlen(str) + 1, sizeof(char));

  char *ptrOne = strCopy;
  char *ptrTwo = NULL;
  char *expandedPtr = expanded;

  // ptrTwo = strstr(strCopy, "$$");

  // if (ptrTwo == NULL)
  // {
  //   free(strCopy);
  //   strCopy = NULL;
  //   return str;
  // }
  // else
  // {
  char *strPid = calloc(6, sizeof(char));
  sprintf(strPid, "%d", getpid());
  // expanded = realloc(expanded, (strlen(expanded) + strlen(strPid) + 1) * sizeof(char));
  // while (ptrOne < ptrTwo)
  // {
  //   *expandedPtr = *ptrOne;
  //   ptrOne++;
  //   expandedPtr++;
  // }
  // strcat(expanded, strPid);

  // ptrTwo = strstr(ptrOne, "$$");
  // ptrTwo = strstr(strCopy, "$$");

  while ((ptrTwo = strstr(strCopy, "$$")) != NULL)
  {
    int strSize = strlen(expanded) + strlen(strPid) + 1;
    int expLength = strlen(expanded);
    int strPidLength = strlen(strPid);
    int bytes = strSize * sizeof(char);
    expanded = (char *)realloc(expanded, strSize * sizeof(char));
    int l = strlen(expanded);
    int s = sizeof(expanded);
    expandedPtr = expanded + strlen(expanded);
    while (ptrOne < ptrTwo)
    {
      *expandedPtr = *ptrOne;
      ptrOne++;
      expandedPtr++;
    }
    strcat(expanded, strPid);
    strCopy = ptrTwo + 2;
    ptrOne = strCopy;
    // move the pointer building the expanded string forward by the length of the pid
    expandedPtr += strlen(strPid);
  }

  if (strCopy != NULL)
  {
    expanded = (char *)realloc(expanded, (strlen(expanded) + strlen(strCopy) + 1) * sizeof(char));
    strcat(expanded, strCopy);
  }

  free(strCopyPtr);
  strCopyPtr = NULL;

  strCopy = NULL;

  ptrOne = NULL;
  ptrTwo = NULL;
  expandedPtr = NULL;

  free(strPid);
  strPid = NULL;

  return expanded;
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
    char *expanded = NULL;
    expanded = handleExpansion(token);
    userInput->args[i] = calloc(strlen(expanded + 1), sizeof(char));
    strcpy(userInput->args[i], expanded);
    token = strtok_r(NULL, " ", &savePtr);
    i++;
    expanded = NULL;
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
