#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "tokenizer.h"

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens* tokens);
int cmd_help(struct tokens* tokens);
int cmd_pwd(struct tokens* tokens);
int cmd_cd(struct tokens* tokens);

int process = 0;

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens* tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t* fun;
  char* cmd;
  char* doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
    {cmd_help, "?", "show this help menu"},
    {cmd_exit, "exit", "exit the command shell"},
    {cmd_pwd, "pwd", "shows current working directory"},
    {cmd_cd, "cd", "changes directory"}
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens* tokens) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens* tokens) { exit(0); }

int cmd_pwd (unused struct tokens* tokens) {
  char pathname[256];
  getcwd(pathname, sizeof(pathname));
  printf("%s\n", pathname);
  return 1;
}

int cmd_cd (struct tokens* tokens) {
  const char *path = tokens_get_token(tokens, tokens_get_length(tokens) - 1);
  chdir(path);
  return 1;
}

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive) {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);
    
    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

int main(unused int argc, unused char* argv[]) {
  init_shell();

  static char line[4096];
  int line_num = 0;

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin)) {
    /* Split our line into words. */
    struct tokens* tokens = tokenize(line);

    /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));
    int count = 0;

    if (tokens_get_length(tokens) > 1) {
        if (strcmp("&", tokens_get_token(tokens, tokens_get_length(tokens) - 1)) == 0) {
            count = 1;
            process++;
        }
    }
    
    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } else {
      /* REPLACE this to run commands as programs. */
      pid_t pd = fork();
      if (pd > 0) {
        int status;
        if (!count) {
            wait(&status);
        }
      }
      if (pd == 0) {
        pid_t childid = getpid();
        setpgid(0, childid);
        if (!count) {
          tcsetpgrp(shell_terminal, childid);
        }
        signal(SIGINT,SIG_DFL);
        signal(SIGQUIT,SIG_DFL);
        signal(SIGTSTP,SIG_DFL);
        signal(SIGTTIN,SIG_DFL);
        signal(SIGTTOU,SIG_DFL);
        signal(SIGCHLD,SIG_DFL);
        signal(SIGCONT,SIG_DFL);
        signal(SIGKILL,SIG_DFL);
        size_t tokenLength = tokens_get_length(tokens);
        int status = 1;
        if (tokenLength > 2) {
          if (strcmp(">", tokens_get_token(tokens, tokenLength - 2)) == 0) {
            int file_des = open(tokens_get_token(tokens, tokenLength - 1), O_RDWR | O_CREAT, S_IRWXU);
            dup2(file_des, 1);
            close(file_des);
            status = 1;
          } else if (strcmp("<", tokens_get_token(tokens, tokenLength - 2)) == 0) {
            int fdd = open(tokens_get_token(tokens, tokenLength - 1), O_RDWR | O_CREAT, S_IRWXU);
            dup2(fdd, 0);
            close(fdd);
            status = 1;
          } else if (strcmp("|", tokens_get_token(tokens, tokenLength - 2)) == 0) {
            int pipe = 0;
            printf("im trying to pipe %d\n", pipe);
            
          } else status = 0;
        } else status = 0;
        if (status) tokenLength -= 2;
        status = 0;
        if (count) {
            status = 1;
        }
        if (status) {
            status = 2;
        }
        tokenLength -= status;
        char *path = tokens_get_token(tokens, 0);
        char *param[tokenLength + 1];
        for (int i = 0; i < tokenLength; i++) {
          param[i] = tokens_get_token(tokens, i);
        }
        param[tokenLength] = NULL;
        if (execv(tokens_get_token(tokens, 0), param) == -1) {
          char *envPath = getenv("PATH");
          char *newPath = strtok(envPath, ":");
          char search[2048];
          while (newPath) {
            strcpy(search, newPath);
            strcat(search, "/");
            strcat(search, tokens_get_token(tokens, 0));
            if (execv(search, param) == -1) {
              newPath = strtok(NULL, ":");
            }
          }
        }
        execv(path, param);
        exit(0);
      }
      //fprintf(stdout, "This shell doesn't know how to run programs.\n");
    }

    tcsetpgrp(shell_terminal, shell_pgid);
    signal(SIGINT,SIG_DFL);
    signal(SIGQUIT,SIG_DFL);
    signal(SIGTSTP,SIG_DFL);
    signal(SIGTTIN,SIG_DFL);
    signal(SIGTTOU,SIG_DFL);
    signal(SIGCHLD,SIG_DFL);
    signal(SIGCONT,SIG_DFL);
    signal(SIGKILL,SIG_DFL);

    if (shell_is_interactive)
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);

    /* Clean up memory */
    tokens_destroy(tokens);
  }

  return 0;
}
        
