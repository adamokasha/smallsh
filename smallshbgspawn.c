#define _GNU_SOURCE

#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <fcntl.h>

#include "smallshtok.h"
#include "smallshstatus.h"
#include "smallshbgspawn.h"

const char devNULL[] = "/dev/null";

void addSpawnPid(pid_t spawnPid, pid_t *spawnPids);
void printSpawnPids(pid_t *spawnPids);

/*
  Spawns a background process (control is returned immediately to the shell)
  with optional input and output redirection

  Citation:
  Adapted from code in CS344 Module 4 "Exploration: Process API - Monitoring Child Processes"
  Source URL: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
*/
void spawnBackgroundProcess(struct UserInput *userInput, struct CommandStatus *commandStatus, pid_t *spawnPids)
{
  int childStatus;
  pid_t spawnPid = fork();
  int sourceFD;
  int targetFD;

  switch (spawnPid)
  {
  case -1:
    perror("Fork error\n");
    fflush(stdout);
    exit(1);
    break;
  case 0:
    /*
      Citation for lines 38-62
      Adapted from CS344 Module 5 "Exploration: Processes and I/O"
      Source URL: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-processes-and-i-slash-o?module_item_id=21835982
    */
    if (userInput->inputFile != NULL)
    {
      sourceFD = open(userInput->inputFile, O_RDONLY);
    }
    else
    {
      sourceFD = open(devNULL, O_RDONLY);
    }
    // Redirect input
    int result = dup2(sourceFD, 0);
    if (result == -1)
    {
      perror("Error");
      fflush(stdout);
      setCommandStatus(commandStatus, FORKED, 1);
      exit(1);
    }
    if (userInput->outputFile != NULL)
    {
      targetFD = open(userInput->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    }
    else
    {
      targetFD = open(devNULL, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    }
    // Redirect output
    int result2 = dup2(targetFD, 1);
    if (result2 == -1)
    {
      perror("Error");
      fflush(stdout);
      setCommandStatus(commandStatus, FORKED, 1);
      exit(1);
    }
    execvp(userInput->command, userInput->args);
    perror("execvp");
    fflush(stdout);
    exit(1);
    break;
  default:
    printf("Background pid is %d\n", spawnPid);
    fflush(stdout);
    addSpawnPid(spawnPid, spawnPids);
    // printSpawnStatus(spawnPids);
    spawnPid = waitpid(spawnPid, &childStatus, WNOHANG);

    // TODO: ??
    if (childStatus != 0)
    {
      setCommandStatus(commandStatus, FORKED, 1);
    }
    else
    {
      setCommandStatus(commandStatus, FORKED, childStatus);
    }

    break;
  }
}

void addSpawnPid(pid_t spawnPid, pid_t *spawnPids)
{
  int i = 0;
  pid_t *pidPtr = spawnPids;

  while (i < 100)
  {
    if (*pidPtr == 0)
    {
      *pidPtr = spawnPid;
      // printf("%d\n", *pidPtr);
      // fflush(stdout);
      break;
    }
    pidPtr++;
    i++;
  }
  pidPtr = NULL;
}

void printSpawnPids(pid_t *spawnPids)
{
  int i = 0;
  pid_t *pidPtr = spawnPids;

  while (i < 100)
  {
    if (*pidPtr != 0)
    {
      printf("Spawn id: %d\n", *pidPtr);
      fflush(stdout);
    }
    pidPtr++;
    i++;
  }
  pidPtr = NULL;
}

void printSpawnStatus(pid_t *spawnPids)
{
  int i = 0;
  pid_t *pidPtr = spawnPids;
  int childStatus;
  pid_t spawnPid;

  while (i < 100)
  {
    if (*pidPtr != 0)
    {
      spawnPid = waitpid(*pidPtr, &childStatus, WNOHANG);
      // printf("Pid: %d | Return value: %d | child status: %d\n", *pidPtr, spawnPid, childStatus);
      // fflush(stdout);

      if (spawnPid > 0) {
        if (WIFEXITED(childStatus)){
          printf("Background pid %d exited normally with exit value %d\n", spawnPid, WEXITSTATUS(childStatus));
          *pidPtr = 0;
        }
      } else if (spawnPid < 0) {
        if (WIFSIGNALED(childStatus)) {
          printf("Background pid %d exited abnormally with exit value %d\n", spawnPid, WTERMSIG(childStatus));
          *pidPtr = 0;
        }
      }
      fflush(stdout);
    }
    pidPtr++;
    i++;
  }

  pidPtr = NULL;
}