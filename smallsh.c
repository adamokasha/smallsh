#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct userInputs {
  char *command;
  char args[512];
  char *inputRedirect;
  char *outputRedirect;
  char *inputFile;
  char *outputFile;
  bool isBackgroundProcess;
};

void prompt(char **buf, size_t *buflen) {
  printf(": ");
  getline(buf, buflen, stdin);
}

int main() {
  char *buf = NULL;
  size_t buflen;

  prompt(&buf, &buflen);
  printf(buf);
  fflush(stdin);

  return EXIT_SUCCESS;
}
