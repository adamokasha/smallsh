void spawnBackgroundProcess(struct UserInput *userInput, struct CommandStatus *commandStatus, pid_t *spawnPids);
void printSpawnStatus(pid_t *spawnPids);
void killBackgroundProcesses(pid_t *spawnPids);