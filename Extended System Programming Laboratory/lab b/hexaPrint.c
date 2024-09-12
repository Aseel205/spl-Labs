#include <stdio.h>
#include <stdlib.h>

// Helper function to print the hexadecimal values of bytes in a buffer
// this part isnt checked yet..
void PrintHex(const unsigned char *buffer, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    // Open the file in binary mode
    FILE *file = fopen(argv[1], "rb");
    if (!file)
    {
        perror("Error opening file");
        return 1;
    }

    // Buffer to store bytes read from the file
    unsigned char buffer[16];  // Reading 16 bytes at a time
    size_t bytesRead;

    // Read the file and print its contents in hexadecimal format
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        PrintHex(buffer, bytesRead);
    }

    // Close the file
    fclose(file);

    return 0;
}
