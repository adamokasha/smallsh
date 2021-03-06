/*
  This file contains code for:
  - Creating a child foreground process and executing a command with that child
  - Setting the status of the child as control is returned to the parent
*/
#define _GNU_SOURCE

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "smallshtok.h"
#include "smallshstatus.h"
#include "smallshfgspawn.h"
#include "smallshsig.h"

/*
  Spawns a foreground process (the parent will wait for command to execute before taking control)
  with optional input and output redirection

  Citation:
  Adapted from code in CS344 Module 4 "Exploration: Process API - Monitoring Child Processes"
  Source URL: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-process-api-monitoring-child-processes?module_item_id=21835973
*/
void spawnForegroundProcess(struct UserInput *userInput, struct CommandStatus *commandStatus)
{
  int childStatus;
  pid_t spawnPid = fork();

  switch (spawnPid)
  {
  case -1:
    perror("Fork error\n");
    fflush(stdout);
    exit(1);
    break;
  case 0:
    register_restore_SIGINT();
    /*
      Citation for lines 49-85
      Adapted from CS344 Module 5 "Exploration: Processes and I/O"
      Here we check if input and output file are available in input struct
      UserInput.`dup2` is then used to redirect input and output to the files
      specified by the user.
      Source URL: https://canvas.oregonstate.edu/courses/1884946/pages/exploration-processes-and-i-slash-o?module_item_id=21835982
    */
    if (userInput->inputFile != NULL)
    {
      int sourceFD = open(userInput->inputFile, O_RDONLY);
      if (sourceFD == -1)
      {
        perror("Input file error");
        fflush(stdout);
        exit(1);
      }
      // Redirect input
      int result = dup2(sourceFD, 0);
      if (result == -1)
      {
        perror("Input redirect error");
        fflush(stdout);
        exit(1);
      }
    }
    if (userInput->outputFile != NULL)
    {
      int targetFD = open(userInput->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
      if (targetFD == -1)
      {
        perror("Output file error");
        fflush(stdout);
        exit(1);
      }
      // Redirect output
      int result2 = dup2(targetFD, 1);
      if (result2 == -1)
      {
        perror("Output file error");
        fflush(stdout);
        exit(1);
      }
    }
    execvp(userInput->command, userInput->args);
    perror("Error");
    fflush(stdout);
    exit(1);
    break;
  default:
    spawnPid = waitpid(spawnPid, &childStatus, 0);

    if (childStatus == SIGINT)
    {
      printf("terminated by signal %d\n", SIGINT);
      fflush(stdout);
      setCommandStatus(commandStatus, FORKED, SIGINT);
    }
    else if (childStatus != 0)
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