// #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "smallshcd.h"
#include "smallshtok.h"

void smallshcd(char *dir)
{
  char *buf = NULL;

  // printf("HOME: %s\n", getenv("HOME"));
  if (dir == NULL)
  {
    // printf("CURRENT DIR: %s\n", getcwd(buf, 48));
    chdir(getenv("HOME"));
    // printf("CURRENT DIR: %s\n", getcwd(buf, 48));
  }
  else
  {
    // printf("CURRENT DIR: %s\n", getcwd(buf, 48));
    chdir(dir);
    // printf("CURRENT DIR: %s\n", getcwd(buf, 48));
  }

  free(buf);
  buf = NULL;
}