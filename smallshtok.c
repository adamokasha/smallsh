#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // getpid

#include "smallshtok.h"
#include "smallshutils.h"

char *handleExpansion(char *str)
{
  char *strCopy = calloc(strlen(str) + 1, sizeof(char));
  strCopy = strcpy(strCopy, str);
  char *strCopyPtr = strCopy;
  char *expanded = calloc(strlen(str) + 1, sizeof(char));

  char *ptrOne = strCopy;
  char *ptrTwo = NULL;
  char *expandedPtr = expanded;

  char *strPid = calloc(6, sizeof(char));
  sprintf(strPid, "%d", getpid());

  while ((ptrTwo = strstr(strCopy, "$$")) != NULL)
  {
    expanded = (char *)realloc(expanded, (strlen(expanded) + strlen(strPid) + 1) * sizeof(char));
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
    userInput->args[i] = calloc(strlen(expanded) + 1, sizeof(char));
    strcpy(userInput->args[i], expanded);
    token = strtok_r(NULL, " ", &savePtr);
    i++;
    expanded = NULL;
  }

  if (isInputCharacter(token))
  {
    token = strtok_r(NULL, " ", &savePtr);
    char *expanded = NULL;
    expanded = handleExpansion(token);

    userInput->inputFile = calloc(strlen(token) + 1, sizeof(char));
    strcpy(userInput->inputFile, expanded);
  }

  token = strtok_r(NULL, " ", &savePtr);

  if (isOutputCharacter(token))
  {
    token = strtok_r(NULL, " ", &savePtr);
    char *expanded = NULL;
    expanded = handleExpansion(token);

    userInput->outputFile = calloc(strlen(token) + 1, sizeof(char));
    strcpy(userInput->outputFile, expanded);
  }

  token = strtok_r(NULL, " ", &savePtr);

  if (isBackgroundExecChar(token))
  {
    userInput->isBackgroundProcess = true;
  }
}
