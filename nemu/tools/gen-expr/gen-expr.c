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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  int result = %s; "
"  printf(\"%%d\", result); "
"  return 0; "
"}";

static inline unsigned choose(unsigned n) {
  return rand() % n;
}

static inline void gen(char c) {
  char add_str[] = {c, '\0'};
  strcat(buf, add_str);
}

static inline void gen_num() {
  char num_str[11];
  sprintf(num_str,"%u",choose((unsigned)(100)));
  strcat(buf, num_str);
}

static inline void gen_rand_op() {
  char op_list[] = {'+', '-', '*','/'};
  char add_str[] = {op_list[choose(4)], '\0'};
  strcat(buf, add_str);
}

static inline void gen_space() {
  for(int i = 0; i < choose(2); i++) {
    if(choose(100) >= 50)
    {
      strcat(buf," ");
    }
  }
}

static inline void gen_rand_expr(int level) {
  gen_space();
  if (level > 0) {
    switch (choose(3)) {
      case 0: gen_num(); break;
      case 1: gen('('); gen_rand_expr(level - 1); gen(')'); break;
      default: gen_rand_expr(level - 1); gen_rand_op(); gen_rand_expr(level - 1); break;
    }
  }
  else {
    gen_num();
  }
  gen_space();
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    memset(buf, 0, 65536);

    gen_rand_expr(rand() % 10);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%d:%s\n", result, buf);
  }
  return 0;
}
