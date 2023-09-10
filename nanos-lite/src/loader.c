#include "fs.h"
#include "memory.h"
#include "proc.h"
#include <elf.h>

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
            #ifdef HAS_VME
            if (pcb) {
                uintptr_t vaddr_start = (uintptr_t)phdr->p_vaddr & 0xfffffffffffff000;
                uintptr_t vaddr_stop = ((uintptr_t)phdr->p_vaddr + phdr->p_memsz - 1) & 0xfffffffffffff000;
                int page_num = ((vaddr_stop - vaddr_start) >> 12) + 1;
                for (int i = 0; i < page_num; i++)
                {
                    map(&pcb->as, (void *)(vaddr_start + i * PGSIZE), (void *)vaddr_start + i * PGSIZE, 0);
                }
                pcb->max_brk = vaddr_stop + PGSIZE;
            }
            #endif
            void *bin = (void *)phdr->p_vaddr;
            fs_lseek(fd, phdr->p_offset, SEEK_SET);
            fs_read(fd, bin, phdr->p_filesz);
            memset(bin + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
        }
    }
    fs_close(fd);
    free(ehdr);
    free(phdr);
    return ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    ((void (*)())entry)();
}

void context_kload(PCB *pcb, void *entry, void *arg) {
    Area kstack = {(void *)pcb->stack, (void *)(pcb->stack + 1)};
    pcb->cp = kcontext(kstack, entry, arg);
}

int strlistlen(char *src[]) {
    int num = 0;
    if (src) {
        while (src[num]) {
            num++;
        }
    }
    return num;
}

#define SIZE_RESTORE 8

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
    Log("load file: %s", filename);
    #ifdef HAS_VME
    protect(&pcb->as);
    #endif
    uintptr_t ustack_base = (uintptr_t)new_page(8);
    uintptr_t ustack =  ustack_base + 8 * PGSIZE;
    #ifdef HAS_VME
    for (int i = 0; i < 8; i++) {
        map(&pcb->as, (void *)ustack_base + i * PGSIZE, (void *)ustack_base + i * PGSIZE, 0);
    }
    #endif
    int argc = strlistlen((char **)argv);
    int envpc = strlistlen((char **)envp);
    int total_length = (argc + envpc + 3) * sizeof(uintptr_t *) + SIZE_RESTORE;
    int pointers_length = total_length;
    for (int i = 0; i < argc; i++) {
        total_length += (strlen(argv[i]) + 1);
    }
    for (int i = 0; i < envpc; i++) {
        total_length += (strlen(envp[i]) + 1);
    }
    total_length += (strlen(filename) + 1);
    total_length = ROUND(total_length, 0x100);
    uintptr_t kargv_pointer = (uintptr_t)ustack - total_length;
    intptr_t kargv = kargv_pointer + pointers_length;

    /* 1. store argc in the lowest */
    *((int *)kargv_pointer) = argc;
    kargv_pointer += sizeof(uint64_t);

    /* 2. save argv pointer in 2nd lowest and argv in lowest strings */
    uintptr_t new_argv = (uintptr_t)ustack - total_length + pointers_length;
    for (int i = 0; i < argc; i++) {
        *((uintptr_t *)kargv_pointer + i) = new_argv;
        strcpy((char *)kargv, argv[i]);
        new_argv += (strlen(argv[i]) + 1);
        kargv += (strlen(argv[i]) + 1);
    }
    *((uintptr_t *)kargv_pointer + argc) = 0;
    kargv_pointer += (argc + 1) * sizeof(uintptr_t);

    /* 3. save envp pointer in 3rd lowest and envp in 2nd lowest strings */
    for (int i = 0; i < envpc; i++) {
        *((uintptr_t *)kargv_pointer + i) = new_argv;
        strcpy((char *)kargv, envp[i]);
        new_argv += strlen(envp[i]) + 1;
        kargv += strlen(envp[i]) + 1;
    }
    *((uintptr_t *)kargv_pointer + envpc) = 0;
    kargv_pointer += (envpc + 1) * sizeof(uintptr_t);

    /* 4. save filename */
    strcpy((char *)kargv, filename);
    new_argv = new_argv + strlen(filename) + 1;
    kargv += strlen(filename) + 1;

    uintptr_t entry = loader(pcb, filename);
    Area kstack = {(void *)pcb->stack, (void *)(pcb->stack + 1)};
    pcb->cp = ucontext(&pcb->as, kstack, (void *)entry);
    pcb->cp->GPRx = ustack - total_length;
}