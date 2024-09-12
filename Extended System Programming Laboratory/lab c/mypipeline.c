#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    int pipefd[2];
    pid_t childp1, childp2;
    if(pipe(pipefd) == -1){
        perror("Error making pipe!");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "(parent_process>forking...)\n");
    childp1 = fork();
    if(childp1 == -1){
        perror("Error forking child1");
        exit(EXIT_FAILURE);
    }
    if(childp1 == 0){
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
        close(STDOUT_FILENO);
        if (dup(pipefd[1]) == -1) {
            perror("dup");
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]);
        char *cmd[] = {"ls", "-l", NULL};
        fprintf(stderr, "(child1>going to execute cmd: %s %s)\n", cmd[0], cmd[1]);
        if(execvp(cmd[0],cmd)== -1)
        {
            perror("error in execv");
            _exit(EXIT_FAILURE);
        }
    }else{
        fprintf(stderr, "(parent_process>created process with id: %d)\n", childp1);
        fprintf(stderr, "(parent_process>forking...)\n");
        fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
        close(pipefd[1]);
        childp2 = fork();

        if(childp2 == -1){
        perror("Error forking child2");
        exit(EXIT_FAILURE);
        }

        if(childp2 == 0){
            fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
        close(STDIN_FILENO);

        if (dup(pipefd[0]) == -1) {
            perror("dup");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        char *cmd[] = {"tail", "-n", "2", NULL};
        fprintf(stderr, "(child2>going to execute cmd: %s %s %s)\n", cmd[0], cmd[1], cmd[2]);
        if(execvp(cmd[0],cmd)== -1)
        {
            perror("error in execv");
            _exit(EXIT_FAILURE);
        }

        }else{
            fprintf(stderr, "(parent_process>created process with id: %d)\n", childp2);
            fprintf(stderr, "(parent_process>closing the read end of the pipe...)\n");
            close(pipefd[0]);

            fprintf(stderr, "(parent_process>waiting for child processes to terminate...)\n");
            waitpid(childp1, NULL, 0);
            waitpid(childp2, NULL, 0);
            fprintf(stderr, "(parent_process>exiting...)\n");
        }

    }
}