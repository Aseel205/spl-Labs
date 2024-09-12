#include <stdio.h>
#include <string.h>
#include <unistd.h>     // For access() function
#include <errno.h>      // For errno variable
#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1

#define BufferSize 8192

extern int system_call(int, ...);
extern void infection();
extern void infector(char *filename);

int main(int argc, char *argv[], char *envp[]) {
    int virus = 0;
    char *fileN = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-a", 2) == 0) {
            fileN = argv[i] + 2;
            virus = 1;
        } else {
            fileN = argv[i];
        }
    }
    
    if (virus == 1 && fileN != NULL) {
        // Check if file exists
        if (access(fileN, F_OK) != -1) {
            system_call(SYS_WRITE, STDOUT, "VIRUS ATTACHED to ", 19);
            system_call(SYS_WRITE, STDOUT, fileN, strlen(fileN));
            system_call(SYS_WRITE, STDOUT, "\n", 1);
            infector(fileN);
            infection();
        } else {
            // File does not exist
            system_call(SYS_WRITE, STDOUT, "Error: File does not exist\n", 26);
        }
    }
    
    return 0;
}
