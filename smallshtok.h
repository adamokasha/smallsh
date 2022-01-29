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