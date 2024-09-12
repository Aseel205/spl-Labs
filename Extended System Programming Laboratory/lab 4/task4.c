#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int count_digits(const char* str);

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Insufficient arguments provided.\n");
        return 1;
    }

    if (argc > 2) {
        printf("Only the first argument will be considered.\n");
    }

    printf("Number of digits: %d\n", count_digits(argv[1]));
    return 0;
}

int count_digits(const char* str) {
    int digit_count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (isdigit((unsigned char)str[i])) {
            digit_count++;
        }
    }
    return digit_count;
}
