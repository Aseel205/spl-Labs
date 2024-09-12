#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

void handler(int sig) {
    printf("\nReceived Signal: %s\n", strsignal(sig));
    if (sig == SIGTSTP) {
        signal(SIGTSTP, SIG_DFL);
    } else if (sig == SIGCONT) {
        signal(SIGCONT, SIG_DFL);
    }
    signal(sig, SIG_DFL);
    raise(sig);
}

int main(int argc, char **argv) {
    pid_t pid = getpid();
    printf("Starting the program with PID: %d\n", pid);
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGCONT, handler);

    while (1) {
        sleep(1);

    }

    return 0;
}
