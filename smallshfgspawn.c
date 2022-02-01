#define _GNU_SOURCE

#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <fcntl.h>

#include "smallshtok.h"
#include "smallshstatus.h"
#include "smallshfgspawn.h"

// TODO: comment + citation
void spawnForegroundProcess(struct UserInput *userInput, struct CommandStatus *commandStatus)
{
  int childStatus;
  pid_t spawnPid = fork();

  switch (spawnPid)
  {
  case -1:
    perror("fork()\n");
    // exit(1);
    break;
  case 0:
    if (userInput->inputFile != NULL)
    {
      int sourceFD = open(userInput->inputFile, O_RDONLY);
      int result = dup2(sourceFD, 0);
      if (result == -1)
      {
        perror("source dup2()");
        exit(1);
      }
    }
    if (userInput->outputFile != NULL)
    {
      int targetFD = open(userInput->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0740);
      int result2 = dup2(targetFD, 1);
      printf("DESC %d", targetFD);
      if (result2 == -1)
      {
        perror("target dup2()");
        exit(2);
      }
    }
    execvp(userInput->command, userInput->args);
    perror("execvp");
    exit(1);
    break;
  default:
    spawnPid = waitpid(spawnPid, &childStatus, 0);

    if (childStatus != 0)
    {
      setCommandStatus(commandStatus, FORKED, 1);
    }
    else
    {
      setCommandStatus(commandStatus, FORKED, childStatus);
    }
    setCommandStatus(commandStatus, FORKED, childStatus);

    // printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
    // exit(0);
    break;
  }
}