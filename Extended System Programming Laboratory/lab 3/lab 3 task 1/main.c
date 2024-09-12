#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1

extern int system_call();

int main(int argc, char* argv[], char* envp[])
{
    /* Loop through each argument in argv and print it */
    for (int i = 0; i < argc; ++i) {
        char *arg = argv[i];
        int len = strlen(arg);

        /* Using system calls to write to STDOUT */
        system_call(SYS_WRITE, STDOUT, (int)arg, len);
        system_call(SYS_WRITE, STDOUT, (int)"\n", 1);
    }

    return 0;
}
