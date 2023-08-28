#include "monitor/symbol.h"
#include "common.h"
#include "monitor/elf.h"

function_symbol_t func_symbols[MAX_FUNCTION_NUM] = {0};
int free_symbol_ptr = 0;

function_symbol_t *find_free_symbols() {
    if (free_symbol_ptr >= MAX_FUNCTION_NUM) {
        return NULL;
    }
    func_symbols[free_symbol_ptr].used = true;
    return &func_symbols[free_symbol_ptr++];
}

function_symbol_t *find_function_symbol(word_t address, bool isret) {
    if (!isret) {
        for (int i = 0; i < free_symbol_ptr; i++) {
            if (func_symbols[i].address == address) {
                return &func_symbols[i];
            }
        }
    } else {
        int idx = 0;
        word_t addr = 0;
        for (int i = 0; i < free_symbol_ptr; i++) {
            if (func_symbols[i].address < address && func_symbols[i].address > addr) {
                addr = func_symbols[i].address;
                idx = i;
            }
        }
        return &func_symbols[idx];
    }
    return NULL;
}

static bool verify_contents(char *contents) {
    if (strncmp(ELFMAG, contents, sizeof(ELFMAG) - 1)) {
        Log("Elf file header error!");
        return false;
    }
    return true;
}

static void read_sections(char *contents) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)contents;
    Assert(*(uint64_t *)ehdr->e_ident == 0x7f454c4602010100, "Elf Magic error");
    // sections
    Elf64_Shdr *sections = (Elf64_Shdr *)(contents + ehdr->e_shoff);
    // symbols
    Elf64_Sym *symbols = NULL;
    // string tables
    char *string_table = NULL, *sh_string_table = NULL;
    int sym_shstr_found = 0, symbol_table_size = 0, first_str_table = 0;
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (sym_shstr_found == 2) {
            break;
        }
        if (sections[i].sh_type == SHT_SYMTAB) {
            // symbol table
            symbols = (Elf64_Sym *)(contents + sections[i].sh_offset);
            symbol_table_size = sections[i].sh_size / sections[i].sh_entsize;
            sym_shstr_found++;
        }
        if (sections[i].sh_type == SHT_STRTAB) {
            // string table
            if (!first_str_table) {
                first_str_table = i;
            }
            sh_string_table = (char *)(contents + sections[i].sh_offset);
            char *section_name = (char *)(sh_string_table + sections[i].sh_name);
            if (strcmp((const char *)section_name, ".shstrtab") == 0) {
                // section header string table
                sym_shstr_found++;
            }
        }
    }
    for (int i = first_str_table; i < ehdr->e_shnum; i++) {
        if (sections[i].sh_type == SHT_STRTAB) {
            // string table
            char *section_name = (char *)(sh_string_table + sections[i].sh_name);
            if (strcmp((const char *)section_name, ".strtab") == 0) {
                // string table
                string_table = (char *)(contents + sections[i].sh_offset);
                break;
            }
        }
    }
    for (int i = 0; i < symbol_table_size; i++) {
        if (ELF64_ST_TYPE(symbols[i].st_info) == STT_FUNC) {
            // function symbol
            function_symbol_t *new = find_free_symbols();
            if (new) {
                new->address = symbols[i].st_value;
                char *func_name = (char *)(string_table + symbols[i].st_name);
                strncpy(new->name, (const char *)func_name, sizeof(new->name));
            } else {
                break;
            }
        }
    }
}

void init_elf(char *file_name) {
    if (file_name == NULL) {
        Log("No elf is given, skip reading elf");
        return;
    }

    FILE *fp = fopen(file_name, "rb");
    Assert(fp, "Can not open '%s'", file_name);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    Log("The elf is %s, size = %ld", file_name, size);

    fseek(fp, 0, SEEK_SET);
    char *elf_contents = (char *)malloc(size);
    int ret = fread(elf_contents, size, 1, fp);
    assert(ret == 1);

    assert(verify_contents(elf_contents) == true);

    read_sections(elf_contents);

    free(elf_contents);
    fclose(fp);
}