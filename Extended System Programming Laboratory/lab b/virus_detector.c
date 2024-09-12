#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

char sigFileName[256] = "signatures-L";

void SetSigFileName();
virus* readVirus(FILE* file);
void printVirus(virus* v);
void list_print(link *virus_list, FILE *stream);
link* list_append(link* virus_list, virus* data);
void list_free(link *virus_list);
void loadSuspectedFile(char *fileName, char **buffer, unsigned int *size);
void detectViruses(char *buffer, unsigned int size, link *virus_list);
void neutralize_virus(char *fileName, int signatureOffset);

int main() {
    link *virus_list = NULL;
    char suspectedFileName[256];
    char *suspectedFileBuffer = NULL;
    unsigned int suspectedFileSize = 0;

    while (1) {
        int choice;
        printf("Menu:\n");
        printf("0) Set signatures file name\n");
        printf("1) Load signatures\n");
        printf("2) Print signatures\n");
        printf("3) Set suspected file name\n");
        printf("4) Detect viruses\n");
        printf("5) Fix file\n");
        printf("6) Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 0:
                SetSigFileName();
                break;
            case 1: {
                FILE* file = fopen(sigFileName, "rb");
                if (!file) {
                    printf("Failed to open file. Make sure the file exists.\n");
                    break;
                }

                char magic[4];
                if (fread(magic, 1, 4, file) != 4) {
                    fprintf(stderr, "Failed to read magic number\n");
                    fclose(file);
                    return 1;
                }

                if (strncmp(magic, "VIRL", 4) != 0 && strncmp(magic, "VIRB", 4) != 0) {
                    fprintf(stderr, "Invalid magic number\n");
                    fclose(file);
                    return 1;
                }

                virus* v;
                while ((v = readVirus(file)) != NULL) {
                    virus_list = list_append(virus_list, v);
                }

                fclose(file);
                printf("Signatures loaded successfully.\n");
                break;
            }
            case 2:
                list_print(virus_list, stdout);
                break;
            case 3:
                printf("Enter the suspected file name: ");
                scanf("%255s", suspectedFileName);
                loadSuspectedFile(suspectedFileName, &suspectedFileBuffer, &suspectedFileSize);
                break;
            case 4:
                detectViruses(suspectedFileBuffer, suspectedFileSize, virus_list);
                break;
            case 5: {
                if (suspectedFileBuffer && suspectedFileSize > 0) {
                    printf("Fixing file...\n");
                    detectViruses(suspectedFileBuffer, suspectedFileSize, virus_list);
                    printf("Enter the virus index to neutralize: ");
                    int virusIndex;
                    scanf("%d", &virusIndex);
                    neutralize_virus(suspectedFileName, virusIndex);
                } else {
                    printf("No suspected file loaded.\n");
                }
                break;
            }
            case 6:
                list_free(virus_list);
                if (suspectedFileBuffer) free(suspectedFileBuffer);
                printf("Exiting...\n");
                                exit(EXIT_SUCCESS);
            default:
                printf("Invalid choice\n");
        }
    }

    return 0;
}

void SetSigFileName() {
    printf("Enter the signature file name: ");
    scanf("%255s", sigFileName);
}

virus* readVirus(FILE* file) {
    virus* v = (virus*)malloc(sizeof(virus));
    if (v == NULL) {
        perror("Failed to allocate memory for virus");
        exit(1);
    }

    if (fread(v, 1, 18, file) != 18) {
        free(v);
        return NULL;
    }

    v->sig = (unsigned char*)malloc(v->SigSize);
    if (v->sig == NULL) {
        perror("Failed to allocate memory for signature");
        free(v);
        exit(1);
    }

    if (fread(v->sig, 1, v->SigSize, file) != v->SigSize) {
        free(v->sig);
        free(v);
        return NULL;
    }

    return v;
}

void printVirus(virus* v) {
    printf("Virus name: %s\n", v->virusName);
    printf("Virus signature length: %d\n", v->SigSize);
    printf("Virus signature:\n");

    for (int i = 0; i < v->SigSize; i++) {
        printf("%02X ", v->sig[i]);
    }
    printf("\n");
}

void list_print(link *virus_list, FILE *stream) {
    while (virus_list != NULL) {
        printVirus(virus_list->vir);
        virus_list = virus_list->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data) {
    link* newLink = (link*)malloc(sizeof(link));
    if (!newLink) {
        perror("Failed to allocate memory for new link");
        exit(EXIT_FAILURE);
    }

    newLink->vir = data;
    newLink->nextVirus = NULL;

    if (!virus_list) {
        return newLink;
    }

    link* current = virus_list;
    while (current->nextVirus != NULL) {
        current = current->nextVirus;
    }
    current->nextVirus = newLink;

    return virus_list;
}

void list_free(link *virus_list) {
    while (virus_list != NULL) {
        link *temp = virus_list;
        virus_list = virus_list->nextVirus;
        free(temp->vir->sig);
        free(temp->vir);
        free(temp);
    }
}

void loadSuspectedFile(char *fileName, char **buffer, unsigned int *size) {
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        printf("Failed to open suspected file. Make sure the file exists.\n");
        return;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    *buffer = (char *)malloc(*size);
    if (!(*buffer)) {
        fclose(file);
        printf("Failed to allocate memory for suspected file.\n");
        return;
    }

    // Read the file into the buffer
    if (fread(*buffer, 1, *size, file) != *size) {
        fclose(file);
        free(*buffer);
        printf("Failed to read suspected file.\n");
        return;
    }

    fclose(file);
}

void detectViruses(char *buffer, unsigned int size, link *virus_list) {
    if (!buffer || size == 0 || !virus_list) {
        printf("Invalid parameters for virus detection.\n");
        return;
    }

    link *current = virus_list;
    while (current != NULL) {
        virus *currentVirus = current->vir;
        unsigned short signatureSize = currentVirus->SigSize;
        unsigned char *signature = currentVirus->sig;
        for (unsigned int i = 0; i <= size - signatureSize; ++i) {
            if (memcmp(buffer + i, signature, signatureSize) == 0) {
                printf("Virus detected!\n");
                printf("Virus name: %s\n", currentVirus->virusName);
                printf("Starting byte location: %u\n", i);
                printf("Size of virus signature: %hu\n", signatureSize);
            }
        }
        current = current->nextVirus;
    }
}

void neutralize_virus(char *fileName, int signatureOffset) {
    FILE *file = fopen(fileName, "r+b");
    if (!file) {
        printf("Failed to open suspected file for modification.\n");
        return;
    }

    // Move the file pointer to the offset
    if (fseek(file, signatureOffset, SEEK_SET) != 0) {
        printf("Failed to seek to the virus location.\n");
        fclose(file);
        return;
    }

    // Replace the first byte with a RET instruction (0xC3)
    unsigned char retInstruction = 0xC3;
    if (fwrite(&retInstruction, sizeof(unsigned char), 1, file) != 1) {
        printf("Failed to write the RET instruction.\n");
        fclose(file);
        return;
    }

    printf("Virus neutralized successfully!\n");

    fclose(file);
}
              
