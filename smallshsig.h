extern int fgOnlyMode;

void register_ignore_SIGINT();
void register_restore_SIGINT();
void register_ignore_SIGTSTP();
void register_toggle_fg_mode();