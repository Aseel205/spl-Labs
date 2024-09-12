#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>

extern int startup(int argc, char **argv, void (*start)());

// Function to print program header information for task 0
void print_task0(Elf32_Phdr *phdr, int index) {
    printf("Program header number %d at address 0x%x\n", index, (unsigned int)phdr);
}

// Function to print detailed program header information for task 1
void print_phdr(Elf32_Phdr *phdr, int index) {
    const char *type;
    switch (phdr->p_type) {
        case PT_NULL:    type = "NULL";    break;
        case PT_LOAD:    type = "LOAD";    break;
        case PT_DYNAMIC: type = "DYNAMIC"; break;
        case PT_INTERP:  type = "INTERP";  break;
        case PT_NOTE:    type = "NOTE";    break;
        case PT_SHLIB:   type = "SHLIB";   break;
        case PT_PHDR:    type = "PHDR";    break;
        default:         type = "UNKNOWN"; break;
    }

    char flags[4] = {' ', ' ', ' ', '\0'};
    if (phdr->p_flags & PF_R) flags[0] = 'R';
    if (phdr->p_flags & PF_W) flags[1] = 'W';
    if (phdr->p_flags & PF_X) flags[2] = 'X';

    // Determine protection and mapping flags
    int protectionFlag = 0;
    int map_flags = MAP_PRIVATE | MAP_FIXED;
    if (phdr->p_flags & PF_W) protectionFlag |= PROT_WRITE;
    if (phdr->p_flags & PF_R) protectionFlag |= PROT_READ;
    if (phdr->p_flags & PF_X) protectionFlag |= PROT_EXEC;
    if (phdr->p_flags & PF_W) map_flags |= MAP_SHARED;

    printf("%s 0x%06x 0x%08x 0x%08x 0x%05x 0x%05x %s 0x%x %d %d\n",
           type, phdr->p_offset, phdr->p_vaddr, phdr->p_paddr,
           phdr->p_filesz, phdr->p_memsz, flags, phdr->p_align, protectionFlag, map_flags);
}

// Function to load a program header for task 2b
void load_phdr(Elf32_Phdr *phdr, int fd) {
    if (phdr->p_type == PT_LOAD) {
        off_t offset = phdr->p_offset & 0xfffff000;
        size_t padding = phdr->p_vaddr & 0xfff;
        int protectionFlag = 0;
        int map_flags = MAP_PRIVATE | MAP_FIXED;
        if (phdr->p_flags & PF_W) protectionFlag |= PROT_WRITE;
        if (phdr->p_flags & PF_R) protectionFlag |= PROT_READ;
        if (phdr->p_flags & PF_X) protectionFlag |= PROT_EXEC;

        void *vadd = (void *)(phdr->p_vaddr & 0xfffff000);
        void *map = mmap(vadd, (phdr->p_memsz + padding), protectionFlag, map_flags, fd, offset);
        if (map == MAP_FAILED) {
            perror("Error mapping segment into memory");
            exit(EXIT_FAILURE);
        }
        print_phdr(phdr, 0);
    }
}

// Function to iterate over each program header and apply a given function
int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr_start = (Elf32_Phdr *)((char *)map_start + ehdr->e_phoff);
    int num_phdrs = ehdr->e_phnum;
    for (int i = 0; i < num_phdrs; i++) {
        Elf32_Phdr *current_phdr = &phdr_start[i];
        func(current_phdr, arg);
    }
    return 0;
}

// Main function
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "The file name is missing\n");
        return 1;
    }
    char *file_name = argv[1];
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    size_t file_size = lseek(fd, 0, SEEK_END);
    void *map_start = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("Error mapping file to memory");
        close(fd);
        return 1;
    }
    printf("Type   Offset  VirtAddr   PhysAddr  FileSiz MemSiz  Flg  Align  ProcFlag  MapFlags\n");

    // Iterate over program headers and apply load_phdr function
    int result = foreach_phdr(map_start, load_phdr, fd);

    close(fd);
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
    startup(argc - 1, argv + 1, (void *)(ehdr->e_entry));

    int res = munmap(map_start, file_size);
    if (res == -1) {
        perror("Error in unmap");
        exit(1);
    }
    return 0;
}
