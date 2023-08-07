/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "monitor/watchpoint.h"
#include "monitor/sdb.h"
#include "monitor/expr.h"

static WP wp_pool[NR_WP] = {};

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].used = false;
    memset(wp_pool[i].expr, 0, NR_WP_EXPR_MAX);
    wp_pool[i].value = 0;
  }
}

/* TODO: Implement the functionality of watchpoint */
void display_wp_pool() {
  for (int i = 0; i < NR_WP; i++) {
    if (!wp_pool[i].used) {
      continue;
    }
    printf("Watch point [%3d]: value: 0x%8x, expr:%s\n", wp_pool[i].NO, wp_pool[i].value, wp_pool[i].expr);
  }
}

void sync_wp() {
  for (int i = 0; i < NR_WP; i++) {
    if (wp_pool[i].used) {
      bool success;
      wp_pool[i].value = expr(wp_pool[i].expr, &success);
    }
  }
}

bool check_wp() {
  bool ret = false;
  for (int i = 0; i < NR_WP; i++) {
    if (wp_pool[i].used) {
      bool success;
      word_t new_value = expr(wp_pool[i].expr, &success);
      if (new_value != wp_pool[i].value) {
        printf("[Watch Point %3d Hit]: old value: 0x%8x, new value: 0x%8x, expr: %s;\n", i, wp_pool[i].value, new_value, wp_pool[i].expr);
        wp_pool[i].value = new_value;
        ret = true;
      }
    }
  }
  return ret;
}


WP* alloc_wp(int *num, char *args) {
  for (int i = 0; i < NR_WP; i++) {
    if (!wp_pool[i].used) {
      wp_pool[i].used = true;
      memcpy(wp_pool[i].expr, args, NR_WP_EXPR_MAX);
      *num = i;
      return &wp_pool[i];
    }
  }
  return NULL;
}

void free_wp(int num) {
  memset(wp_pool[num].expr, 0, NR_WP_EXPR_MAX);
  wp_pool[num].value = 0;
  wp_pool[num].used = false;
}