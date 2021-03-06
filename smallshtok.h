#include <stdbool.h>

struct UserInput
{
  char *command;
  char *args[512];
  char *inputFile;
  char *outputFile;
  bool isBackgroundProcess;
};

char *handleExpansion(char *str);
void buildUserInput(char *buf, struct UserInput *userInput);
void initializeUserInput(struct UserInput *userInput);
void freeUserInput(struct UserInput *userInput);