enum CommandType {
  BUILTIN,
  FORKED
};

struct CommandStatus {
  enum CommandType type;
  int statusCode;
};

void setCommandStatus(struct CommandStatus *commandStatus, enum CommandType type, int statusCode);
void printCommandStatus(struct CommandStatus *commandStatus);