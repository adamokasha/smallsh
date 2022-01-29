#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "smallshutils.h"

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

bool isComment(char *buf)
{
  if (buf[0] == '#')
    return true;
  return false;
}

bool isInputCharacter(char *character)
{
  if (character == NULL)
    return false;
  return strcmp(character, ">") == 0;
}

bool isOutputCharacter(char *character)
{
  if (character == NULL)
    return false;
  return strcmp(character, "<") == 0;
}

bool isBackgroundExecChar(char *character)
{
  if (character == NULL)
    return false;
  return strcmp(character, "&") == 0;
}

bool isArgument(char *str)
{
  if (str == NULL)
    return false;
  return !(isInputCharacter(str) || isOutputCharacter(str) || isBackgroundExecChar(str));
}
