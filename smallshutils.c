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
