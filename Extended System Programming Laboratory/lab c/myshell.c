#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <sys/wait.h>
#include "fcntl.h"

#define HISTLEN 20
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process
{
    cmdLine *cmd;
    pid_t pid;
    int status;
    struct process *next;
} process;
process *process_list = NULL;
//cmd struct func copy
cmdLine *CloneCmdLine(const cmdLine *pCmdLine)
{
    if (pCmdLine)
    {
        cmdLine *newpCmdLine = (cmdLine *)malloc(sizeof(cmdLine));
        memcpy(newpCmdLine, pCmdLine, sizeof(cmdLine));
        newpCmdLine->inputRedirect = NULL;
        if (pCmdLine->inputRedirect)
        {
            newpCmdLine->inputRedirect = strdup(pCmdLine->inputRedirect);
        }
        newpCmdLine->outputRedirect = NULL;
        if (pCmdLine->outputRedirect)
        {
            newpCmdLine->outputRedirect = strdup(pCmdLine->outputRedirect);
        }
        newpCmdLine->next = NULL;
        for (int i = 0; i < pCmdLine->argCount; i++)
            newpCmdLine->arguments[i] = strdup(pCmdLine->arguments[i]);
        return newpCmdLine;
    }
    return NULL;
}

void freeProcessList(process *process_list)
{
    process *current = process_list;
    while (current != NULL)
    {
        process *temp = current;
        current = current->next;
        freeCmdLines(temp->cmd);
        free(temp);
    }
}

void updateProcessStatus(process *process_list, int pid, int status)
{
    process *current = process_list;
    while (current != NULL)
    {
        if (current->pid == pid)
        {
            current->status = status;
            return;
        }
        current = current->next;
    }
    printf("Process with pid %d doesn't exist\n", pid);
}

void updateProcessList(process **process_list)
{
    process *current = *process_list;
    pid_t pid;
    while (current != NULL)
    {
        pid = waitpid(current->pid, NULL, WNOHANG);
        if (pid == -1)
        {
            current = current->next;
            continue;
        }
        if (pid == current->pid)
        {
            current->status = TERMINATED;
            printf("Process %d has terminated\n", current->pid);
        }
        current = current->next;
    }
}

void addProcess(process **process_list, cmdLine *pcmd, pid_t pid)
{
    process *new_process = (process *)malloc(sizeof(process));
    new_process->cmd = CloneCmdLine(pcmd);
    new_process->pid = pid;
    new_process->status = RUNNING;
    new_process->next = *process_list;
    *process_list = new_process;
}

void RemoveTerminatedProcses(process **process_list, process *term_process)
{
    process *temp_list = *process_list;
    process *connected_process = NULL;
    while (temp_list != NULL)
    {
        if (temp_list->pid == term_process->pid)
        {
            if (connected_process == NULL)
            {
                *process_list = temp_list->next;
            }
            else
            {
                connected_process->next = temp_list->next;
            }
            free(temp_list);
            return;
        }
        connected_process = temp_list;
        temp_list = temp_list->next;
    }
}
void printProcessList(process **process_list)
{
    updateProcessList(process_list);
    process *temp_process_list = *process_list;
    printf("PID\t\tCommand\t\t\tSTATUS\n");
    printf("---\t\t-------\t\t\t------\n");
    while (temp_process_list != NULL)
    {
        printf("%d\t\t%s\t\t\t", temp_process_list->pid, temp_process_list->cmd->arguments[0]);
        switch (temp_process_list->status)
        {
        case RUNNING:
            printf("Running\n");
            break;
        case SUSPENDED:
            printf("Suspend\n");
            break;
        case TERMINATED:
            printf("Terminated\n");
            process *term_process = temp_process_list;
            RemoveTerminatedProcses(process_list, term_process);
            break;
        }
        temp_process_list = temp_process_list->next;
    }
}
process *ProcessNodeCreate(cmdLine *pcmdline, pid_t pid, int status)
{
    process *link = (process *)malloc(sizeof(process));
    link->cmd = pcmdline;
    link->pid = pid;
    link->status = status;
    link->next = NULL;
    return link;
}

void Duplicate(cmdLine *pCmdLine)
{
    if (pCmdLine->inputRedirect)
    {
        int input_fd = open(pCmdLine->inputRedirect, O_RDONLY);
        if (input_fd == -1)
        {
            perror("can't open input file");
            exit(1);
        }
        if (dup2(input_fd, STDIN_FILENO) == -1)
        {
            perror("error duplicating to input");
            exit(1);
        }
        close(input_fd);
    }
    if (pCmdLine->outputRedirect)
    {
        int output_fd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (output_fd == -1)
        {
            perror("can't open output file");
            exit(1);
        }
        if (dup2(output_fd, STDOUT_FILENO) == -1)
        {
            perror("error duplicating to output");
            exit(1);
        }
        close(output_fd);
    }
}

void execute(cmdLine *pCmdLine)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        _exit(1);
    }
    else if (pid == 0)
    {
        // Child process started executing command
        Duplicate(pCmdLine);
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
        {
            perror("execvp");
            exit(1);
        }
    }
    else
    {
        addProcess(&process_list, pCmdLine, pid);
        // Parent process waiting for child process to finish executing command
        if (pCmdLine->blocking)
        {
            updateProcessStatus(process_list, pid, TERMINATED);
            if (waitpid(pid, NULL, 0) == -1)
            {
                perror("waitpid");
                exit(1);
            }
        }
    }
    // Child process finishes executing command
}

void Print_History_queue(char *history_queue[], int *newest, int *oldest)
{
    for (int i = (*oldest); i <= (*newest); i++)
    {
        printf("%d: %s\n", i - (*oldest) + 1, history_queue[i % HISTLEN]);
    }
}
void Add_To_History(char *history_queue[], char *pCmdLine, int *newest, int *oldest)
{
    char *copy_space = (char *)malloc(strlen(pCmdLine) + 1);
    strcpy(copy_space, pCmdLine);
    if (newest - oldest + 1 > HISTLEN)
    {
        free(history_queue[*oldest]);
        (*oldest)++;
    }
    *newest = (*newest + 1) % HISTLEN;
    history_queue[*newest] = copy_space;
}

void mypipeline(cmdLine *pCmdLine, cmdLine *NextpCmdLine)
{
    int read_write[2];
    if (pipe(read_write) == -1)
    {
        perror("wrong in pipe\n");
        exit(1);
    }
    pid_t child_1 = fork();
    if (child_1 == -1)
    {
        perror("wrong in creating child_1\n");
        exit(1);
    }
    else if (child_1 == 0)
    {
        close(STDOUT_FILENO);
        dup(read_write[1]);
        close(read_write[1]);
        if (pCmdLine->inputRedirect)
        {
            int inp_f = open(pCmdLine->inputRedirect, O_RDONLY);
            if (inp_f == -1)
            {
                perror("cant open input file");
                exit(1);
            }
            if (dup2(inp_f, STDIN_FILENO) == -1)
            {
                perror("error dupplicate to input");
                exit(1);
            }
            close(inp_f);
        }
        if (pCmdLine->outputRedirect != NULL)
        {
            perror("the output doesnt closed\n");
            exit(1);
        }

        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
        { //creats new prosses
            perror("wrong in  child_1 execlp creating new prosses\n");
            exit(1);
        }
    }
    else
    {
        addProcess(&process_list, pCmdLine, child_1);
        updateProcessStatus(process_list, child_1, TERMINATED);
        //closing the parent write-end
        close(read_write[1]);
        pid_t child_2 = fork();
        if (child_2 == -1)
        {
            perror("wrong in creating child_2\n");
            exit(1);
        }
        else if (child_2 == 0)
        {

            close(STDIN_FILENO);
            dup(read_write[0]);
            close(read_write[0]);
            if (NextpCmdLine->inputRedirect != NULL)
            {
                perror("the input doesnt closed\n");
                exit(1);
            }
            if (NextpCmdLine->outputRedirect)
            {
                int out_f = open(NextpCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                if (out_f == -1)
                {
                    perror("cant open output file");
                    exit(1);
                }
                if (dup2(out_f, STDOUT_FILENO) == -1)
                {
                    perror("error dupplicate to output");
                    exit(1);
                }
                close(out_f);
            }

            if (execvp(NextpCmdLine->arguments[0], NextpCmdLine->arguments) == -1)
            { //creats new prosses
                perror("wrong in  child_2 execlp creating new prosses\n");
                exit(1);
            }
        }
        else
        {
            addProcess(&process_list, NextpCmdLine, child_2);
            updateProcessStatus(process_list, child_2, TERMINATED);
            //closing the parent read-end
            close(read_write[0]);
            waitpid(child_1, NULL, 0);
            waitpid(child_2, NULL, 0);
        }
    }
}
void debug_function(cmdLine *pCmdLine)
{
    int from = 0;
    for (int i = 0; i < (pCmdLine->argCount); i++)
    {
        if (strcmp(pCmdLine->arguments[i], "-d") == 0)
        {
            from = i + 1;
            break;
        }
    }
    for (; from < (pCmdLine->argCount); from++)
    {
        if (strcmp(pCmdLine->arguments[from], "-d") == 0)
        {
            continue;
        }
        fprintf(stderr, "%s ", pCmdLine->arguments[from]);
    }
    printf("\n");
}

int There_Is_d(char **argv, int argc)
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            return 1;
        }
    }
    return 0;
}
void alarmProcess(pid_t pid)
{
    if (kill(pid, SIGCONT) == -1)
    {
        perror("kills sigcont failed");
    }
    else
    {
        updateProcessStatus(process_list, pid, RUNNING);
        printf("the Process %d has been alarmed \n", pid);
    }
}
void suspendProcess(pid_t pid)
{
    if (kill(pid, SIGTSTP) == -1)
    {
        perror("kills sigtstp failed");
    }
    else
    {
        updateProcessStatus(process_list, pid, SUSPENDED);
        printf("the Process %d has been suspended\n", pid);
    }
}
void blastProcess(pid_t pid)
{
    if (kill(pid, SIGINT) == -1)
    {
        perror("kills sigkill failed");
    }
    else
    {
        updateProcessStatus(process_list, pid, TERMINATED);
        printf("the Process %d has been blasted\n", pid);
    }
}

int main(int argc, char **argv)
{
    char *history_queue[HISTLEN];  // herer weh have the history queue
    int newest = -1;
    int oldest = 0;
    char current_work[PATH_MAX];
    char line[2048];
    cmdLine *pCmdLine;
    int exists = There_Is_d(argv, argc);

    while (1)
    {
        if (getcwd(current_work, sizeof(current_work)) == NULL)
        {
            perror("error getcwd");
            exit(1);
        }
        fprintf(stderr, "%s> ", current_work);
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            perror("error fgets");
            exit(1);
        }

        if (strcmp(line, "\n") == 0)
        {
            continue;
        }
        line[strcspn(line, "\n")] = '\0';
        Add_To_History(history_queue, line, &newest, &oldest);

        if (strcmp(line, "!!") == 0)
        {
            if (newest == oldest)
            {
                fprintf(stderr, "There is no previous history\n");
                break;
            }
            else
            {
                strcpy(line, history_queue[newest - 1]);
                printf("%s\n", line);
            }
        }

        if (line[0] == '!' && line[1] != '!')
        {
            char num_str[1024];
            strncpy(num_str, line + 1, strlen(line) - 1);
            num_str[strlen(line) - 1] = '\0'; // Null-terminate the copied string
            int n = atoi(num_str);

            if (n >= 1 && n <= newest)
            {
                strcpy(line, history_queue[n - 1]);
                printf("%s\n", history_queue[n - 1]);
            }
            else
            {
                printf("------\nThere is something error in printing the !n history commands\n------\n");
                continue;
            }
        }

        pCmdLine = parseCmdLines(line);

        if (exists)
        {
            fprintf(stderr, "PID is: %d\n", getpid());
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
        }

        if (strcmp(pCmdLine->arguments[0], "cd") == 0)
        {
            if (chdir(pCmdLine->arguments[1]) == -1)
            {
                perror("chdir");
            }
            freeCmdLines(pCmdLine);
            continue;
        }

        if (strcmp(pCmdLine->arguments[0], "quit") == 0)
        {
            printf("Exiting myshell...\n");
            freeProcessList(process_list);
            exit(0);
        }
        else if (strcmp(pCmdLine->arguments[0], "history") == 0)
        {
            Print_History_queue(history_queue, &newest, &oldest);
        }
        else if (pCmdLine->next)
        {
            mypipeline(pCmdLine, pCmdLine->next);
        }
        else if (strcmp(pCmdLine->arguments[0], "suspend") == 0)
        {
            if (pCmdLine->argCount > 1)
            {
                suspendProcess(atoi(pCmdLine->arguments[1]));
            }
            else
            {
                printf("There is no id to suspend\n");
            }
        }
        else if (strcmp(pCmdLine->arguments[0], "alarm") == 0)
        {
            if (pCmdLine->argCount > 1)
            {
                alarmProcess(atoi(pCmdLine->arguments[1]));
            }
            else
            {
                printf("There is no id to alarm\n");
            }
        }
        else if (strcmp(pCmdLine->arguments[0], "procs") == 0)
        {
            printProcessList(&process_list);
        }
        else if (strcmp(pCmdLine->arguments[0], "blast") == 0)
        {
            if (pCmdLine->argCount > 1)
            {
                blastProcess(atoi(pCmdLine->arguments[1]));
            }
            else
            {
                printf("There is no id to blast\n");
            }
        }
        else
        {
            execute(pCmdLine);
        }

        freeCmdLines(pCmdLine);
    }

    freeProcessList(process_list);
    return 0;
}
