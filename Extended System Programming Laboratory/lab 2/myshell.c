#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <signal.h>
#include "LineParser.h"
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 2048

int debug_mode = 0;

void execute(cmdLine *pCmdLine) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {  // Child process
        if (debug_mode) {
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
        }

        // Redirect input if needed
        if (pCmdLine->inputRedirect != NULL) {
            int fd_in = open(pCmdLine->inputRedirect, O_RDONLY);
            if (fd_in == -1) {
                perror("open input file failed");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                perror("dup2 input file descriptor failed");
                close(fd_in);
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }

        // Redirect output if needed
        if (pCmdLine->outputRedirect != NULL) {
            int fd_out = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd_out == -1) {
                perror("open output file failed");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2 output file descriptor failed");
                close(fd_out);
                exit(EXIT_FAILURE);
            }
            close(fd_out);
        }

        execvp(pCmdLine->arguments[0], pCmdLine->arguments);

        // If execvp fails, print error and exit
        perror("execvp failed");
        _exit(EXIT_FAILURE);
    } else {  // Parent process
        if (debug_mode) {
            fprintf(stderr, "Parent PID: %d\n", getpid());
        }

        // Wait for child if blocking
        if (pCmdLine->blocking) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

int main(int argc, char *argv[]) {
    char input[MAX_INPUT_SIZE];
    char cwd[PATH_MAX];
    cmdLine *parsedLine;

    // Check for debug mode
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug_mode = 1;
    }

    while (1) {
        // Display the prompt if there's no input redirection
        if (isatty(STDIN_FILENO)) {
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s> ", cwd);
                fflush(stdout); // Ensure prompt is displayed immediately
            } else {
                perror("getcwd error");
                exit(EXIT_FAILURE);
            }
        }

        // Read input from the user
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            if (feof(stdin)) {
                break; // End of file reached
            } else {
                perror("fgets error");
                continue;
            }
        }

        // Remove trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // Parse the input
        parsedLine = parseCmdLines(input);
        if (parsedLine == NULL) {
            continue;
        }

        // Exit the shell if "quit" is entered
        if (strcmp(parsedLine->arguments[0], "quit") == 0) {
            freeCmdLines(parsedLine);
            break;
        }

        // Handle built-in commands
        if (strcmp(parsedLine->arguments[0], "alarm") == 0) {
            // Handle alarm command
            if (parsedLine->argCount != 2) {
                fprintf(stderr, "Usage: alarm <process id>\n");
            } else {
                pid_t target_pid = atoi(parsedLine->arguments[1]);
                if (kill(target_pid, SIGCONT) == 0) {
                    printf("Process %d woke up successfully\n", target_pid);
                } else {
                    perror("kill failed");
                }
            }
            freeCmdLines(parsedLine);
            continue;
        }

        if (strcmp(parsedLine->arguments[0], "blast") == 0) {
            // Handle blast command
            if (parsedLine->argCount != 2) {
                fprintf(stderr, "Usage: blast <process id>\n");
            } else {
                pid_t target_pid = atoi(parsedLine->arguments[1]);
                if (kill(target_pid, SIGKILL) == 0) {
                    printf("Process %d terminated successfully\n", target_pid);
                } else {
                    perror("kill failed");
                }
            }
            freeCmdLines(parsedLine);
            continue;
        }

        if (strcmp(parsedLine->arguments[0], "cd") == 0) {
            // Handle cd command
            if (parsedLine->argCount != 2) {
                fprintf(stderr, "Usage: cd <directory>\n");
            } else {
                if (chdir(parsedLine->arguments[1]) == -1) {
                    perror("chdir failed");
                }
            }
            freeCmdLines(parsedLine);
            continue;
        }

        // Execute the command
        execute(parsedLine);

        // Release the resources
        freeCmdLines(parsedLine);
    }

    return 0;
}
