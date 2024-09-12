#include <stdlib.h>
#include <stdio.h>
#include <string.h>

 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  /* TODO: Complete during task 2.a */
 if (mapped_array == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < array_length; i++) {
        mapped_array[i] = f(array[i]);
    }


  return mapped_array;
}

char my_get(char c) {
    return fgetc(stdin);
}

char cprt(char c) {
    if (c >= 0x20 && c <= 0x7E) {
        putchar(c);
        putchar('\n');
    } else {
        putchar('.');
        putchar('\n');
    }
    return c;
}

char encrypt(char c) {
    if (c >= 0x20 && c <= 0x4E) {
        return c + 0x20;
    } else {
        return c;
    }
}

char decrypt(char c) {
    if (c >= 0x40 && c <= 0x7E) {
        return c - 0x20;
    } else {
        return c;
    }
}

int xoprt(int c) {
    printf("%x ", c); 
    printf("%o\n", c); 
    return c;          
}
 
 
int main(int argc, char **argv){
  /* TODO: Test your code */
char arr1[] = {'H','E','Y','!'};
char* arr2 = map(arr1, 4, xoprt);
free(arr2);
}
