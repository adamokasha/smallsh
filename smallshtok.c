#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // getpid

#include "smallshtok.h"
#include "smallshutils.h"

/*
  Expands $$ into the PID of currently running process
  Takes each argument separately

  This function uses a copy of string passed in and 3 pointers. Output is in `expanded`.
  1. Point `ptrOne` to copy of string
  Loop:
    2. Point `ptrTwo` to result of finding substring $$ (the pointer result)
    3. Reallocate memory for `expanded` increased size of $$ -> PID expansion
    4. Backtrace `ptrOne` to copy all chars leading to `ptrTwo` (if needed)
    5. Add Pids to `expanded`
*/
char *handleExpansion(char *str)
{
  // Use copy of string passed in
  char *strCopy = calloc(strlen(str) + 1, sizeof(char));
  strCopy = strcpy(strCopy, str);
  // This pointer is used to later free strCopy
  char *strCopyPtr = strCopy;
  // The expanded string to return
  char *expanded = calloc(strlen(str) + 1, sizeof(char));

  /*
    ptrOne is used to backtrace the characters to copy before reaching an expansion ($$)
    ptrTwo points to where an expansion is encountered
    expandedPtr is used to point to where to copy the chars pointed to by ptrOne (usually the end)
  */
  char *ptrOne = strCopy;
  char *ptrTwo = NULL;
  char *expandedPtr = expanded;

  char *strPid = calloc(6, sizeof(char));
  sprintf(strPid, "%d", getpid());

  /*
    Citation
    Date accessed: 01/29/2022
    Code adapted for this use case from post by user Jonathan Leffler
    Citation: https://stackoverflow.com/questions/13482519/c-find-all-occurrences-of-substring
  */
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

  // If we've reached the end via strtok_r, but strCopy is not null
  // the last part of the string needs to be copied (this occurs when
  // the end of the string resembles $$somecharacters, where we want
  // to preserve "somecharacters"
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
/*
  Sets the UserInput member command for easy access and readability
*/
void setCommandToUserInput(char *buf, struct UserInput *userInput)
{
  char *bufCopy = calloc(strlen(buf) + 1, sizeof(char));
  strcpy(bufCopy, buf);
  char *token = NULL;
  char *savePtr = NULL;

  token = strtok_r(bufCopy, " ", &savePtr);

  userInput->command = calloc(strlen(token) + 1, sizeof(char));
  strcpy(userInput->command, token);

  free(bufCopy);
  bufCopy = NULL;
}

/*
  Sets the UserInput members inputFile and outputFile based on
  the result of strtok_r

  Expects that the string has already been parsed passed the arguments
*/
void setRedirectionToUserInput(struct UserInput *userInput, char *token, char **savePtr)
{
  if (isInputCharacter(token))
  {
    token = strtok_r(NULL, " ", savePtr);
    char *expanded = NULL;
    expanded = handleExpansion(token);

    userInput->inputFile = calloc(strlen(expanded) + 1, sizeof(char));
    strcpy(userInput->inputFile, expanded);
  }
  else if (isOutputCharacter(token))
  {
    token = strtok_r(NULL, " ", savePtr);
    char *expanded = NULL;
    expanded = handleExpansion(token);

    userInput->outputFile = calloc(strlen(expanded) + 1, sizeof(char));
    strcpy(userInput->outputFile, expanded);
  }
}

/*
  This function uses handleExpansion and util functions to
  place user input into UserInput struct
*/
void buildUserInput(char *buf, struct UserInput *userInput)
{
  char *bufCopy = calloc(strlen(buf) + 1, sizeof(char));
  strcpy(bufCopy, buf);
  char *token = NULL;
  char *savePtr = NULL;

  // Add command to userInput->command for readability and easy access
  setCommandToUserInput(buf, userInput);

  token = strtok_r(bufCopy, " ", &savePtr);

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

  // < and > symbols can be ordered with either preceding 
  // the other, so setRedirectionToUserInput is called twice
  setRedirectionToUserInput(userInput, token, &savePtr);
  token = strtok_r(NULL, " ", &savePtr);
  setRedirectionToUserInput(userInput, token, &savePtr);

  token = strtok_r(NULL, " ", &savePtr);

  if (isBackgroundExecChar(token))
  {
    userInput->isBackgroundProcess = true;
  }
}
/*
  Initialize UserInput struct members to NULL

  This is to avoid the app aborting when calling
  free on an invalid pointer which can happen on
  repeated blank or comment input
*/
void initializeUserInput(struct UserInput *userInput)
{
  userInput->command = NULL;

  int i = 0;
  while (i < 512)
  {
    userInput->args[i] = NULL;
    i++;
  }

  userInput->inputFile = NULL;
  userInput->outputFile = NULL;
  userInput->isBackgroundProcess = NULL;
}

/*
  Free memory allocated to UserInput and members
*/
void freeUserInput(struct UserInput *userInput)
{
  if (userInput->command != NULL)
  {
    free(userInput->command);
    userInput->command = NULL;
  }

  int i = 0;
  while (i < 512)
  {
    if (userInput->args[i] != NULL)
    {
      free(userInput->args[i]);
      userInput->args[i] = NULL;
    }
    i++;
  }

  if (userInput->inputFile != NULL)
  {
    free(userInput->inputFile);
    userInput->inputFile = NULL;
  }

  if (userInput->outputFile != NULL)
  {
    free(userInput->outputFile);
    userInput->outputFile = NULL;
  }

  userInput->isBackgroundProcess = NULL;

  free(userInput);
  userInput = NULL;
}