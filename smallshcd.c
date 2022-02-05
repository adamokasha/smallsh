/*
  This file contains code for implementing the `cd` built-in command
*/
#include <stdlib.h>
#include <unistd.h>

#include "smallshcd.h"
#include "smallshtok.h"

/*
  Implementation of `cd`. Changes directory

  TODO: directory not found
*/
void smallshcd(char *dir)
{
  char *buf = NULL;

  if (dir == NULL)
  {
    chdir(getenv("HOME"));
  }
  else
  {
    chdir(dir);
  }

  free(buf);
  buf = NULL;
}