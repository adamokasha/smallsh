#define _GNU_SOURCE

#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork

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
    execlp(userInput->command, userInput->command, userInput->args);
    // execlp("ls", "ls");
    perror("execl");
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