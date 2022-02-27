# smallsh

Shell implementing a small subset of features of other well-known shells.

## Compiling
With gcc:
```
gcc -Wall -std=c99 -g3 smallsh.c smallshutils.c smallshtok.c smallshcd.c smallshstatus.c smallshfgspawn.c smallshbgspawn.c smallshsig.c -o smallsh
```

## Features
- Provides a prompt for running commands
- Handles blank lines and comments, which are lines beginning with the # character
- Provides expansion for the variable $$
- Executes 3 commands exit, cd, and status via code built into the shell
- Executes other commands by creating new processes using a function from the exec family of functions
- Supports input and output redirection
- Supports running commands in foreground and background processes
- Implements custom handler SIGINT 
  - A child running as a foreground process must terminate itself when it receives SIGINT
  - Parent process ignores SIGINT 
- Implements custom handler for SIGTSTP
  - A child, if any, running as a foreground process ignores SIGTSTP 
  - Children running as background processes ignore SIGTSTP
  - Parent process receiving SIGSTP for the first time enters foreground only mode (no background processes allowed using & operator)
  - If the user sends SIGTSTP again, shell then returns back to the normal condition where the & operator is once again honored for subsequent commands

## Sample Program Execution
<img width="900" alt="Screen Shot 2022-02-26 at 8 41 40 PM" src="https://user-images.githubusercontent.com/18629771/155864816-e01d1ecb-ac39-467e-bd80-e1f7d6294b30.png">
