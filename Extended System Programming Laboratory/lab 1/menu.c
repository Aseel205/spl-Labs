#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fun_desc {
    char *name;
    char (*fun)(char);
};

char* map(char *array, int array_length, char (*f)(char)) {
    char* mapped_array = (char*)(malloc(array_length * sizeof(char)));
    if (mapped_array == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < array_length; i++) {
        mapped_array[i] = f(array[i]);
    }

    // Print the new array
    printf("Mapped array:(this is for debugging)  ");
    for (int i = 0; i < array_length; i++) {
        printf("%c ", mapped_array[i]);
    }
    printf("\n");

    return mapped_array;
}



char my_get(char c) {
return(fgetc(stdin));

}


 char cprt(char c) {
    if (c >= 0x20 && c <= 0x7E) {
        printf("%c\n", c);
    } else {
        printf(".\n");
    }
    return c;
}




char encrypt(char c) {
    if (c >= 0x20 && c <= 0x4E) {  // here yoy can change the e with 0 
        return c + 0x20; 
    } else {
        return c; // Return c unchanged if it doesn't meet the condition
    }
}

char decrypt(char c) {
    if (c >= 0x40 && c <= 0x7E) {  // here you can  change the  e  with 0
        return c - 0x20; 
    } else {
        return c; 
    }
}



char xoprt(char c) {
    if (c != '\n') { // Check if the character is not newline (Enter)
    printf("%02X\t%03o\n", c, c); // Print hexadecimal and octal representations followed by a newline
    return c; // Return the character unchanged
}
  }
 
int main() {
    struct fun_desc menu[] = {
        {"Get String", my_get},
        {"Print String (cprt)", cprt},
        {"Encrypt", encrypt},
        {"Decrypt", decrypt},
        {"Print Hex (xoprt)", xoprt},
        {NULL, NULL}

    };

    int base_len = 5;
char *arr1=(char*)(malloc(5*sizeof(char)));

    while (1) {
        printf("Select operation from the following menu (ctrl^D for exit):\n");
        for (int i = 0; menu[i].name != NULL; i++) {
            printf("%d) %s\n", i, menu[i].name);
        }

        int choice;
        printf("Option : ");
        scanf("%d", &choice);
        getchar(); // Consume newline character

        if (choice < 0 || choice >= sizeof(menu) / sizeof(menu[0]) - 1) {
            printf("Not within bounds\n");
            break;
        } else {
            printf("Within bounds\n");
             arr1 = map(arr1, base_len, menu[choice].fun);
            printf("DONE.\n");
        }
    }
            free(arr1);

    return 0;
}
