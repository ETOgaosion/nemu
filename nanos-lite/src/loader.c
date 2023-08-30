#include <elf.h>
#include <proc.h>
#include "fs.h"

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
#define EXPECT_TYPE EM_386
#elif defined(__ISA_MIPS__)
#define EXPECT_TYPE EM_MIPS
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
#define EXPECT_TYPE EM_RISCV
#else
#error Unsupported ISA
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
    Elf_Ehdr *ehdr = (Elf_Ehdr *)malloc(sizeof(Elf_Ehdr));
    int fd = fs_open(filename, 0, 0);
    fs_read(fd, (void *)ehdr, sizeof(Elf_Ehdr));
    Assert(*(uint64_t *)ehdr->e_ident == 0x10102464c457f, "[loader] error magic, e_ident: %lx", *(uint64_t *)ehdr->e_ident);
    Assert(ehdr->e_machine == EXPECT_TYPE, "[loader] error machine, expect type: %d, e_machine: %d", EXPECT_TYPE, ehdr->e_machine);
    Elf_Phdr *phdr = (Elf_Phdr *)malloc(sizeof(Elf_Phdr));
    for (int i = 0; i < ehdr->e_phnum; i++) {
        fs_lseek(fd, ehdr->e_phoff + i * ehdr->e_phentsize, SEEK_SET);
        fs_read(fd, (void *)phdr, ehdr->e_phentsize);
        if (phdr->p_type == PT_LOAD) {
            void *bin = (void *)phdr->p_vaddr;
            fs_lseek(fd, phdr->p_offset, SEEK_SET);
            fs_read(fd, bin, phdr->p_filesz);
            memset(bin + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
        }
    }
    free(ehdr);
    free(phdr);
    return ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    ((void (*)())entry)();
}
