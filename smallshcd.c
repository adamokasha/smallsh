/*
  This file contains code for implementing the `cd` built-in command
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "smallshcd.h"
#include "smallshtok.h"

/*
  Implementation of `cd`. Changes directory
*/
void smallshcd(char *dir)
{
  char *buf = NULL;
  int result;

  if (dir == NULL)
  {
    chdir(getenv("HOME"));
  }
  else
  {
    result = chdir(dir);
    if (result == -1)
    {
      perror("Error");
      fflush(stdout);
    }
  }

  free(buf);
  buf = NULL;
}