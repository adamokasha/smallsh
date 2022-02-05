/*
  This file contains code for setting the CommandStatus struct whose
  members will hold information about the last foreground command
  executed
*/
#include <stdio.h>

#include "smallshstatus.h"

/*
  Sets CommandStatus struct members
  type (FORKED/BUILTIN) and statusCode
  for use in printing the status of a command
  on exit
*/
void setCommandStatus(
    struct CommandStatus *commandStatus,
    enum CommandType type,
    int statusCode)
{
  commandStatus->type = type;
  commandStatus->statusCode = statusCode;
}

/*
  Prints the CommandStatus struct member
  statusCode if the last command executed
  was a child process (FORKED)
*/
void printCommandStatus(struct CommandStatus *commandStatus)
{
  if (commandStatus->type == BUILTIN)
  {
    return;
  }

  printf("exit value: %d\n", commandStatus->statusCode);
  fflush(stdout);
}