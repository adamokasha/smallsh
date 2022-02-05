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

/*
  Spawns a background process (control is returned immediately to the shell)
  with optional input and output redirection

  If input and/or output file is not specified, defaults to /dev/null for either or both

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
      Citation for lines 46-79
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

      // If we got back a spawn id greater than zero, we check for termination
      if (spawnPid > 0)
      {
        if (WIFEXITED(childStatus))
        {
          printf("Background pid %d exited normally with exit value %d\n", spawnPid, WEXITSTATUS(childStatus));
          *pidPtr = 0;
        }
      }
      // if we get back a spawn id less than 0, then the child terminated abnormally so we print a message
      else if (spawnPid < 0)
      {
        if (WIFSIGNALED(childStatus))
        {
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