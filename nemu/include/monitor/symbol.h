#pragma once

#include <common.h>

#define MAX_NAME_LEN 20

#define MAX_FUNCTION_NUM 10000

typedef struct function_symbol {
    bool used;
    char name[MAX_NAME_LEN];
    word_t address;
    char file_name[MAX_NAME_LEN];
} function_symbol_t;

extern function_symbol_t func_symbols[MAX_FUNCTION_NUM];

void init_elf(char *file_name);
function_symbol_t *find_function_symbol(word_t address, bool isret);