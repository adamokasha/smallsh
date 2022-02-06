/*
  This file contains code for:
  - Creating a background child and executing a command in that child
  - Keeping track of all the background processes created in an array
  - Checking the status of background children and printing how they were terminated and the
    status code or signal with which they were terminated
*/
#define _GNU_SOURCE

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "smallshtok.h"
#include "smallshstatus.h"
#include "smallshbgspawn.h"
#include "smallshsig.h"

const char devNULL[] = "/dev/null";

void addSpawnPid(pid_t spawnPid, pid_t *spawnPids);

/*
  Spawns a background process (control is returned immediately to the shell)
  with optional input and output redirection

  If input and/or output file is not specified, defaults to `/dev/null` for either or both

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
    register_ignore_SIGTSTP();
    /*
      Citation for lines 59-104
      Adapted from CS344 Module 5 "Exploration: Processes and I/O"
      Here we check if input and output file are available in input struct
      UserInput. If not use `/dev/null` for either/both. `dup2` is then used
      to redirect input and output to the user entries or defaults.
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
    if (sourceFD == -1)
    {
      perror("Input file error");
      fflush(stdout);
      exit(1);
    }
    // Redirect input
    int result = dup2(sourceFD, 0);
    // handle redirection error
    if (result == -1)
    {
      perror("Input redirect error");
      fflush(stdout);
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
    if (targetFD == -1)
    {
      perror("Output file error");
      fflush(stdout);
      exit(1);
    }
    // Redirect output
    int result2 = dup2(targetFD, 1);
    // handle redirection error
    if (result2 == -1)
    {
      perror("Output redirect error");
      fflush(stdout);
      exit(1);
    }
    execvp(userInput->command, userInput->args);
    perror("Error");
    fflush(stdout);
    exit(1);
    break;
  default:
    printf("Background pid is %d\n", spawnPid);
    fflush(stdout);
    addSpawnPid(spawnPid, spawnPids);
    spawnPid = waitpid(spawnPid, &childStatus, WNOHANG);

    break;
  }
}

/*
  Adds a spawn pid to array of pid_t's (spawnPids)
*/
void addSpawnPid(pid_t spawnPid, pid_t *spawnPids)
{
  int i = 0;
  pid_t *pidPtr = spawnPids;

  while (i < 100)
  {
    if (*pidPtr == 0)
    {
      *pidPtr = spawnPid;
      break;
    }
    pidPtr++;
    i++;
  }
  pidPtr = NULL;
}

/*
  Checks spawnPids array for child pids and prints a message
  if the child process was termianted
*/
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
      // If spawnPid is 0 then no status to report
      spawnPid = waitpid(*pidPtr, &childStatus, WNOHANG);

      // If we got back a spawn id other than zero, we check for termination status
      if (spawnPid != 0)
      {
        // Check if a normal exit
        if (WIFEXITED(childStatus))
        {
          printf("Background pid %d exited normally with exit value %d\n", spawnPid, WEXITSTATUS(childStatus));
          *pidPtr = 0;
        }
        // Check if exited via signal and print message indicating which signal caused exit
        if (WIFSIGNALED(childStatus))
        {
          printf("Background pid %d exited with signal %d\n", spawnPid, WTERMSIG(childStatus));
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

/*
  Kills background processes that were not terminated and
  saved to spawnPids array
*/
void killBackgroundProcesses(pid_t *spawnPids)
{
  int i = 0;
  pid_t *pidPtr = spawnPids;

  while (i < 100)
  {
    if (*pidPtr != 0)
    {
      kill(*pidPtr, SIGKILL);
    }

    pidPtr++;
    i++;
  }

  pidPtr = NULL;
}