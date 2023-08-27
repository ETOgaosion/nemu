#pragma once

#include "monitor/sdb.h"

#define NR_WP 32
#define NR_WP_EXPR_MAX 3000

typedef struct watchpoint {
    int NO;
    bool used;

    /* TODO: Add more members if necessary */
    char expr[NR_WP_EXPR_MAX];
    unsigned value;

} WP;

void init_wp_pool();
void display_wp_pool();
void sync_wp();
bool check_wp();
WP *alloc_wp(int *num, char *args);
void free_wp(int num);