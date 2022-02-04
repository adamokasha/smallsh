#include <stdio.h>

#include "smallshstatus.h"

void setCommandStatus(
    struct CommandStatus *commandStatus,
    enum CommandType type,
    int statusCode)
{
  commandStatus->type = type;
  commandStatus->statusCode = statusCode;
}

void printCommandStatus(struct CommandStatus *commandStatus)
{
  if (commandStatus->type == BUILTIN)
  {
    return;
  }

  printf("exit value: %d\n", commandStatus->statusCode);
  fflush(stdout);
}