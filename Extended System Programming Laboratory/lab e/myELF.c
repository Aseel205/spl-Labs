#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <elf.h>
#include <sys/stat.h>

// Menu option structure
typedef struct {
    int number;
    char description[256];
} MenuOption;

// Symbol information structure
typedef struct {
    int index;
    Elf32_Addr value;
    int section_index;
    char section_name[256];
    char symbol_name[256];
} SymbolInfo;

// Function declarations
void toggleDebugMode(int *debugMode);
void examineELFFile();
void printSectionNames();
void printSymbols();
void checkFilesForMerge();
void mergeELFFiles();
void quitProgram();

// Global variables
int debugMode = 0;
int fd1 = -1;
int fd2 = -1;
void *mapped_file1 = NULL;
void *mapped_file2 = NULL;
off_t file_size1;
off_t file_size2;
char file_name1[256];
char file_name2[256];
char input_buffer[256];
int examine_count = 0;

// Menu options array
MenuOption menu[] = {
    {0, "Toggle Debug Mode"},
    {1, "Examine ELF File"},
    {2, "Print Section Names"},
    {3, "Print Symbols"},
    {4, "Check Files for Merge"},
    {5, "Merge ELF Files"},
    {6, "Quit"}
};

// Main function
int main() {
    int option;

    while (1) {
        printf("Choose action:\n");
        for (int i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
            printf("%d-%s\n", menu[i].number, menu[i].description);
        }
        printf("Option: ");
        scanf("%d", &option);
        getchar(); // Consume newline character

        switch (option) {
            case 0:
                toggleDebugMode(&debugMode);
                break;
            case 1:
                if (examine_count >= 2) {
                    printf("Error: Already examined two ELF files. Cannot examine more.\n");
                } else {
                    examineELFFile();
                }
                break;
            case 2:
                printSectionNames();
                break;
            case 3:
                printSymbols();
                break;
            case 4:
                checkFilesForMerge();
                break;
            case 5:
                mergeELFFiles();
                break;
            case 6:
                quitProgram();
                return 0;
            default:
                printf("Invalid option. Please choose a valid option.\n");
        }

        if (debugMode) {
            printf("Debug mode is ON\n");
            printf("fd1: %d, fd2: %d\n", fd1, fd2);
            printf("mapped_file1: %p, mapped_file2: %p\n", mapped_file1, mapped_file2);
            printf("file_size1: %ld, file_size2: %ld\n", file_size1, file_size2);
            printf("file_name1: %s, file_name2: %s\n", file_name1, file_name2);
        }
        printf("\n");
    }
}

// Toggle debug mode function
void toggleDebugMode(int *debugMode) {
    *debugMode = !(*debugMode);
    printf("Debug mode %s\n", *debugMode ? "ON" : "OFF");
}

// Examine ELF file function
void examineELFFile() {
    if (examine_count >= 2) {
        printf("Error: Already examined two ELF files. Cannot examine more.\n");
        return;
    }

    printf("Enter the name of the ELF file: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    input_buffer[strcspn(input_buffer, "\n")] = '\0';

    int fd = open(input_buffer, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    struct stat sb;
    if (fstat(fd, &sb) < 0) {
        perror("fstat");
        close(fd);
        return;
    }

    if (examine_count == 0) {
        strcpy(file_name1, input_buffer);
        mapped_file1 = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapped_file1 == MAP_FAILED) {
            perror("mmap");
            close(fd);
            return;
        }
        file_size1 = sb.st_size;
        fd1 = fd;
    } else {
        strcpy(file_name2, input_buffer);
        mapped_file2 = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapped_file2 == MAP_FAILED) {
            perror("mmap");
            close(fd);
            return;
        }
        file_size2 = sb.st_size;
        fd2 = fd;
    }

    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)mapped_file1;
    if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Error: Not a valid ELF file.\n");
        munmap(mapped_file1, sb.st_size);
        close(fd);
        mapped_file1 = NULL;
        fd1 = -1;
        return;
    }

    printf("Bytes 1, 2, 3 of the magic number (in ASCII): %c%c%c\n",
           elf_header->e_ident[EI_MAG1], elf_header->e_ident[EI_MAG2], elf_header->e_ident[EI_MAG3]);
    printf("Data encoding scheme of the object file: %s\n",
           (elf_header->e_ident[EI_DATA] == ELFDATA2LSB) ? "Little Endian" : "Big Endian");
    printf("Entry point (hexadecimal address): 0x%x\n", elf_header->e_entry);
    printf("File offset of the section header table: 0x%x\n", elf_header->e_shoff);
    printf("Number of section header entries: %d\n", elf_header->e_shnum);
    printf("Size of each section header entry: %d bytes\n", elf_header->e_shentsize);
    printf("File offset of the program header table: 0x%x\n", elf_header->e_phoff);
    printf("Number of program header entries: %d\n", elf_header->e_phnum);
    printf("Size of each program header entry: %d bytes\n", elf_header->e_phentsize);

    examine_count++;
}

// Print section names function
void printSectionNames() {
    if (fd1 == -1 && fd2 == -1) {
        printf("Error: no ELF files have been opened.\n");
        return;
    }

    if (fd1 != -1) {
        Elf32_Ehdr *elf_header1 = (Elf32_Ehdr *)mapped_file1;
        Elf32_Shdr *section_header_table1 = (Elf32_Shdr *)((uintptr_t)mapped_file1 + elf_header1->e_shoff);
        char *string_table1 = (char *)((uintptr_t)mapped_file1 + section_header_table1[elf_header1->e_shstrndx].sh_offset);

        printf("File %s\n", file_name1);
        for (int i = 0; i < elf_header1->e_shnum; i++) {
            printf("[%d] %s 0x%x 0x%x 0x%x %d\n", i, &string_table1[section_header_table1[i].sh_name],
                   section_header_table1[i].sh_addr, section_header_table1[i].sh_offset,
                   section_header_table1[i].sh_size, section_header_table1[i].sh_type);
        }
        printf("---------------\n");
    }

    if (fd2 != -1) {
        Elf32_Ehdr *elf_header2 = (Elf32_Ehdr *)mapped_file2;
        Elf32_Shdr *section_header_table2 = (Elf32_Shdr *)((uintptr_t)mapped_file2 + elf_header2->e_shoff);
        char *string_table2 = (char *)((uintptr_t)mapped_file2 + section_header_table2[elf_header2->e_shstrndx].sh_offset);

        printf("File %s\n", file_name2);
        for (int i = 0; i < elf_header2->e_shnum; i++) {
            printf("[%d] %s 0x%x 0x%x 0x%x %d\n", i, &string_table2[section_header_table2[i].sh_name],
                   section_header_table2[i].sh_addr, section_header_table2[i].sh_offset,
                   section_header_table2[i].sh_size, section_header_table2[i].sh_type);
        }
        printf("---------------\n");
    }
}

// Print symbols function
void printSymbols() {
    if (fd1 == -1 && fd2 == -1) {
        printf("Error: No ELF file has been examined yet.\n");
        return;
    }

    if (fd1 != -1) {
        Elf32_Ehdr *elf_header = (Elf32_Ehdr *)mapped_file1;
        Elf32_Shdr *section_header_table = (Elf32_Shdr *)((uintptr_t)mapped_file1 + elf_header->e_shoff);
        char *string_table = (char *)((uintptr_t)mapped_file1 + section_header_table[elf_header->e_shstrndx].sh_offset);

        printf("File %s\n", file_name1);

        for (int i = 0; i < elf_header->e_shnum; i++) {
            if (section_header_table[i].sh_type == SHT_SYMTAB) {
                Elf32_Sym *symbol_table = (Elf32_Sym *)((uintptr_t)mapped_file1 + section_header_table[i].sh_offset);
                int symbol_count = section_header_table[i].sh_size / section_header_table[i].sh_entsize;
                char *symbol_string_table = (char *)((uintptr_t)mapped_file1 + section_header_table[section_header_table[i].sh_link].sh_offset);

                printf("Symbol table '%s' contains %d entries:\n", &string_table[section_header_table[i].sh_name], symbol_count);
                printf("[Index] Value Section_index Section_name Symbol_name\n");
                for (int j = 0; j < symbol_count; j++) {
                    printf("[%d] 0x%x %d %s %s\n", j, symbol_table[j].st_value, symbol_table[j].st_shndx,
                           &string_table[section_header_table[symbol_table[j].st_shndx].sh_name], &symbol_string_table[symbol_table[j].st_name]);
                }
            }
        }
        printf("---------------\n");
    }

    if (fd2 != -1) {
        Elf32_Ehdr *elf_header = (Elf32_Ehdr *)mapped_file2;
        Elf32_Shdr *section_header_table = (Elf32_Shdr *)((uintptr_t)mapped_file2 + elf_header->e_shoff);
        char *string_table = (char *)((uintptr_t)mapped_file2 + section_header_table[elf_header->e_shstrndx].sh_offset);

        printf("File %s\n", file_name2);

        for (int i = 0; i < elf_header->e_shnum; i++) {
            if (section_header_table[i].sh_type == SHT_SYMTAB) {
                Elf32_Sym *symbol_table = (Elf32_Sym *)((uintptr_t)mapped_file2 + section_header_table[i].sh_offset);
                int symbol_count = section_header_table[i].sh_size / section_header_table[i].sh_entsize;
                char *symbol_string_table = (char *)((uintptr_t)mapped_file2 + section_header_table[section_header_table[i].sh_link].sh_offset);

                printf("Symbol table '%s' contains %d entries:\n", &string_table[section_header_table[i].sh_name], symbol_count);
                printf("[Index] Value Section_index Section_name Symbol_name\n");
                for (int j = 0; j < symbol_count; j++) {
                    printf("[%d] 0x%x %d %s %s\n", j, symbol_table[j].st_value, symbol_table[j].st_shndx,
                           &string_table[section_header_table[symbol_table[j].st_shndx].sh_name], &symbol_string_table[symbol_table[j].st_name]);
                }
            }
        }
        printf("---------------\n");
    }
}

// Check files for merge function
void checkFilesForMerge() {
    if (fd1 == -1 || fd2 == -1) {
        printf("Error: Need to examine two ELF files before merging.\n");
        return;
    }

    Elf32_Ehdr *elf_header1 = (Elf32_Ehdr *)mapped_file1;
    Elf32_Ehdr *elf_header2 = (Elf32_Ehdr *)mapped_file2;

    if (elf_header1->e_type != elf_header2->e_type) {
        printf("Error: ELF files are of different types and cannot be merged.\n");
        return;
    }

    if (elf_header1->e_machine != elf_header2->e_machine) {
        printf("Error: ELF files are for different architectures and cannot be merged.\n");
        return;
    }

    printf("Files %s and %s are compatible for merging.\n", file_name1, file_name2);
}

// Merge ELF files function
void mergeELFFiles() {
    // Implement the ELF file merging functionality here
    printf("Merge ELF Files function not implemented yet.\n");
}

// Quit program function
void quitProgram() {
    if (mapped_file1 != NULL) {
        munmap(mapped_file1, file_size1);
        close(fd1);
    }

    if (mapped_file2 != NULL) {
        munmap(mapped_file2, file_size2);
        close(fd2);
    }

    printf("Exiting the program.\n");
    exit(0);
}
