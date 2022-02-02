#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "smallshutils.h"

/*
  Removes a trailing "\n" from character string
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

/*
  Checks if a string is empty
  Uses empty space as delimiter to strtok_r
*/
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
    return true;
  }

  free(bufCopy);
  bufCopy = NULL;

  return false;
}

/*
  Checks if char is a comment
*/
bool isComment(char *buf)
{
  if (buf[0] == '#')
    return true;
  return false;
}

/*
  Checks if char is ">" or input symbol
*/
bool isInputCharacter(char *character)
{
  if (character == NULL)
    return false;
  return strcmp(character, "<") == 0;
}

/*
  Checks if char is "<" or output symbol
*/
bool isOutputCharacter(char *character)
{
  if (character == NULL)
    return false;
  return strcmp(character, ">") == 0;
}

/*
  Checks if char is "&" or use background process flag
*/
bool isBackgroundExecChar(char *character)
{
  if (character == NULL)
    return false;
  return strcmp(character, "&") == 0;
}

/*
  Checks if str is arguments in entire user input
  ie. If it is neither input symbol, output symbol or background exec flag
*/
bool isArgument(char *str)
{
  if (str == NULL)
    return false;
  return !(isInputCharacter(str) || isOutputCharacter(str) || isBackgroundExecChar(str));
}
