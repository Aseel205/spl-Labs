#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For atoi()

void extract_encoding_key(char *arg, int *encode_mode, int *encoding_key, int **digits, int *key_digit_count) {
    *encoding_key = atoi(arg + 2); 
    int temp = *encoding_key;
    *key_digit_count = 0; 
    while (temp != 0) {
        temp /= 10;
        (*key_digit_count)++;
    }
    // Allocate memory for the digits array
    *digits = (int *)malloc(*key_digit_count * sizeof(int));
  
    
    for (int i = *key_digit_count - 1; i >= 0; i--) {
        (*digits)[i] = *encoding_key % 10;
        *encoding_key /= 10;
    }
}

void check_arguments(int argc, char **argv, int *debug_mode, int *encode_mode, int *encoding_key, int **digits, int *key_digit_count, FILE **infile, FILE **outfile) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-D") == 0) {
            *debug_mode = -1; // Turn of debug mode
        } else if (strcmp(argv[i], "+D") == 0) {
            *debug_mode = 1; // Turn on debug mode
        } else if (strncmp(argv[i], "+e", 2) == 0) {
            *encode_mode = 1; // Set encoding mode to addition
            extract_encoding_key(argv[i], encode_mode, encoding_key, digits, key_digit_count);
        } else if (strncmp(argv[i], "-e", 2) == 0) {
            *encode_mode = -1; // Set encoding mode to subtraction
            extract_encoding_key(argv[i], encode_mode, encoding_key, digits, key_digit_count);
        } else if (strncmp(argv[i], "-I", 2) == 0) {
            *infile = fopen(argv[i] + 2, "r");
            if (*infile == NULL) {    // set the infile according to the input
                fprintf(stderr, "Unable to open input file %s\n", argv[i] + 2);
                return;
            } else {
                printf("Opened input file: %s\n", argv[i] + 2);
            }
        } else if (strncmp(argv[i], "-O", 2) == 0) {
            *outfile = fopen(argv[i] + 2, "w");
            if (*outfile == NULL) {  // set the outfile according to the output
                fprintf(stderr, "Unable to open output file %s\n", argv[i] + 2);
                return;
            }
        }
    }
}


int main(int argc, char **argv) {
    printf("The program has started\n");
    int debug_mode = 1; // Default debug mode is off
    int encode_mode = 0; // Default encoding mode is off
    int encoding_key = 0;
    int *digits = NULL; // Array to store the digits
    int key_digit_count = 0; // Variable to store the number of digits in the encoding ke
    FILE *infile = stdin; // Default input stream is stdin
    FILE *outfile = stdout; // Default output stream is stdout
    check_arguments(argc, argv, &debug_mode, &encode_mode, &encoding_key, &digits, &key_digit_count, &infile, &outfile);


    // Check if debug mode is on
    if (debug_mode==1) {
        printf("Debug mode is on\n");
    }
    int c;
    int counter = 0;

        // Read input characters and encode them  
        // now we enter an infinte loop
    while ((c = fgetc(infile)) != EOF) {
        // Print input character if debug mode is on
       

        // Apply encoding if encoding mode is enabled
        if (encode_mode != 0 && (c >= '0' && c <= '9' || c >= 'a' && c <= 'z')) {
            int key_digit = digits[counter];
            counter++;
            if (counter == key_digit_count)
                counter = 0;

            if (encode_mode == 1) { // Addition encoding
                // Handling digits greater than '9'
                if (c >= '0' && c <= '9') {
                    c -= '0'; // Convert digit to its numerical value
                    c = (c + key_digit) % 10 + '0'; // Apply addition with wrap around
                } else {
                    c += key_digit;
                    if (c > 'z') // Wrap around if necessary
                        c -= 26;
                }
            } else { // Subtraction encoding
                // Handling digits greater than '9'
                if (c >= '0' && c <= '9') {
                    c -= '0'; // Convert digit to its numerical value
                    c = (c - key_digit + 10) % 10 + '0'; // Apply subtraction with wrap around
                } else {
                    c -= key_digit;
                    if (c < 'a') // Wrap around if necessary
                        c += 26;
                }
            }
        }

     
        
        if(debug_mode==1)
        fputc(c, stdout); 

        fputc(c, outfile); 

    }
}
