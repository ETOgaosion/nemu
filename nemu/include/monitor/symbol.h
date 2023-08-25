#pragma once

#include <common.h>

#define MAX_FUNCTION_NUM 1000

typedef struct function_symbol {
    bool used;
    char name[16];
    word_t address;
} function_symbol_t;

extern function_symbol_t func_symbols[MAX_FUNCTION_NUM];

void init_elf(char *file_name);
function_symbol_t *find_function_symbol(word_t address, bool isret);