#include "fs.h"
#include "memory.h"
#include "proc.h"
#include <arch/riscv64-nemu.h>
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
            void *pg = (void *)(phdr->p_vaddr & 0xfffffffffffff000);
#ifdef HAS_VME
            if (pcb) {
                uintptr_t vaddr_start = (uintptr_t)pg;
                uintptr_t vaddr_end = ((uintptr_t)phdr->p_vaddr + phdr->p_memsz - 1) & 0xfffffffffffff000;
                int page_num = ((vaddr_end - vaddr_start) >> 12);
                if (phdr->p_vaddr & 0xfff) {
                    page_num++;
                }
                if ((phdr->p_vaddr + phdr->p_memsz) & 0xfff) {
                    page_num++;
                }
                pg = new_page(page_num);
                for (int i = 0; i < page_num; i++) {
                    map(&pcb->as, (void *)vaddr_start + i * PGSIZE, pg + i * PGSIZE, _PAGE_EXEC | _PAGE_WRITE | _PAGE_READ | _PAGE_USER);
                }
                pcb->max_brk = vaddr_end + PGSIZE;
            }
#endif
            void *bin = pg + (phdr->p_vaddr & 0xfff);
            fs_lseek(fd, phdr->p_offset, SEEK_SET);
            fs_read(fd, bin, phdr->p_filesz);
            memset(bin + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
        }
    }
    uintptr_t ret = ehdr->e_entry;
    fs_close(fd);
    free(ehdr);
    free(phdr);
    return ret;
}

void naive_uload(PCB *pcb, const char *filename) {
    uintptr_t entry = loader(pcb, filename);
    ((void (*)())entry)();
}

void context_kload(PCB *pcb, void *entry, void *arg) {
    Area kstack = {(void *)pcb->stack, (void *)pcb->stack + 8 * PGSIZE};
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
    uintptr_t ustack = ustack_base + 8 * PGSIZE;
#ifdef HAS_VME
    for (int i = 0; i < 8; i++) {
        map(&pcb->as, (void *)pcb->as.area.end - 8 * PGSIZE + i * PGSIZE, (void *)ustack_base + i * PGSIZE, _PAGE_EXEC | _PAGE_WRITE | _PAGE_READ | _PAGE_USER);
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
    Area kstack = {(void *)pcb->stack, (void *)pcb->stack + 8 * PGSIZE};
    pcb->cp = ucontext(&pcb->as, kstack, (void *)entry);
    pcb->cp->GPRx = (uintptr_t)pcb->as.area.end - total_length;
}