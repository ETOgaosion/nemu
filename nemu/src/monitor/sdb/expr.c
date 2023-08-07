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

#include <isa.h>
#include "monitor/sdb.h"
#include "memory/paddr.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#define EXPR_MAX_LENGTH 3000

enum {
  TK_NOTYPE,

  /* TODO: Add more token types */
  TK_NUM, TK_HEX, TK_REG,
  TK_BRACKET_LEFT, TK_BRACKET_RIGHT,
  TK_PLUS, TK_MINUS, TK_MUL, TK_DIV,
  TK_NEG, TK_EQ, TK_NEQ, TK_DEREF,
};

enum {
  SINGLE_OPERAND, DOUBLE_OPERAND
};
static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  
  {"[0-9]+", TK_NUM},
  {"0x[0-9a-fA-F]+", TK_HEX},
  {"\\$[0-9a-zA-Z]+", TK_REG},
  
  {"\\(", TK_BRACKET_LEFT},
  {"\\)", TK_BRACKET_RIGHT},
  
  {"\\+", TK_PLUS},
  {"-", TK_MINUS},
  {"\\*", TK_MUL},
  {"/", TK_DIV},

  // {"-", TK_NEG},
  {"==", TK_EQ},
  {"!=", TK_NEQ},
  // {"*", TK_DEREF},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

static int token_priority(int TOKEN) {
  switch (TOKEN) {
    case TK_BRACKET_LEFT: case TK_BRACKET_RIGHT: return 4;
    case TK_NEG: case TK_DEREF: return 3;
    case TK_MUL: case TK_DIV: return 2;
    case TK_PLUS: case TK_MINUS: return 1;
    case TK_EQ: case TK_NEQ: return 0;
    default: return -1;
  }
}

static bool token_left_combine(int TOKEN) {
  switch (TOKEN) {
    case TK_NEG: case TK_DEREF: return false;
    default: return true;
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

#define NR_TOKEN_MAX_NUM 100

static Token tokens[NR_TOKEN_MAX_NUM] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static void same_token_clearify() {
  if (nr_token > 0) {
    switch (tokens[nr_token - 1].type)
    {
      case TK_BRACKET_LEFT:
      case TK_EQ:
      case TK_NEQ:
        break;
      
      default:
        return;
        break;
    }
  }
  switch(tokens[nr_token].type) {
    case '-': tokens[nr_token].type = TK_NEG; break;
    case '*': tokens[nr_token].type = TK_DEREF; break;
    default: break;
  }
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (nr_token + 1 >= EXPR_MAX_LENGTH) {
          panic("[expr token] The expression is too long!");
        }
        if (rules[i].token_type > TK_REG) {
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          same_token_clearify();
          nr_token++;
        }
        else if (rules[i].token_type != TK_NOTYPE) {
          if(substr_len > 31) {
            panic("[expr token] The number is too big!");
          }
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          tokens[nr_token].str[substr_len] = '\0';
          nr_token++;
          break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int p, int q) {
  if (q <= p + 1) {
	  return false;
  }
  if (tokens[p].type != TK_BRACKET_LEFT || tokens[q].type != TK_BRACKET_RIGHT) {
	  return false;
  }
  int parentheses_number = 0;
  for (int i = p + 1; i <= q - 1; i++) {
    if (tokens[i].type == TK_BRACKET_LEFT) {
      parentheses_number++;
    }
    else if(tokens[i].type ==TK_BRACKET_RIGHT) {
      if (parentheses_number <= 0) {
        return false;
      }
      else {
        parentheses_number--;
      }
    }
  }
  if (parentheses_number == 0) {
	  return true;
  }
  else {
	  return false;
  }
}

static int find_main_op(int p, int q) {
  int main_op = -1;
	int parentheses_number = 0;
	for (int i = p; i <= q; i++) {
    if (parentheses_number > 0 && tokens[i].type != TK_BRACKET_RIGHT && tokens[i].type != TK_BRACKET_LEFT) {
      continue;
    }
	  switch(tokens[i].type) {
      case TK_NUM: case TK_HEX: case TK_REG:
        continue;
        break;
      case TK_BRACKET_LEFT:
        parentheses_number++;
        break;
      case TK_BRACKET_RIGHT:
        parentheses_number--;
        break;
      default: {
	      int check_priority = 0;
        if (main_op > 0) {
          check_priority = token_priority(tokens[i].type) - token_priority(tokens[main_op].type);
        }
        if (parentheses_number == 0) {
          if (token_left_combine(tokens[i].type)) {
            if (main_op == -1 || check_priority <= 0) {
              main_op = i;
            }
          }
          else {
            if (main_op == -1 || check_priority > 0) {
              main_op = i;
            }
          }
        }
        break;
      }
    }
	}
  return main_op;
}

static word_t eval (int p, int q) {
  if (p < 0 || q < 0 || p > q) {
	  panic("Bad expression!");
    return -1;
  }
  else if (p == q) {
    unsigned tmp;
    switch (tokens[p].type) {
      case TK_HEX: 
        sscanf(tokens[p].str, "%x", &tmp);
        break;
      case TK_NUM:
        tmp = atoi(tokens[p].str);
        break;
      case TK_REG: {
        bool success = false;
        tmp = isa_reg_str2val(tokens[p].str + 1, &success);
        if (!success) {
          panic("[expr eval] Wrong Register!");
        }
        break;
      }
      default:
        panic("[expr eval] Bad number!");
        break;
    }
    return tmp;
  }
  else if (check_parentheses(p, q)) {
    return eval(p + 1, q - 1);
  }
  else {
    int main_op = find_main_op(p, q);
    word_t val1 = 0;
    word_t val2 = 0;
    word_t ret = 0;
    if (main_op > p) {
      val1 = eval(p, main_op - 1);
    }
    val2 = eval(main_op + 1, q);
    // Evaluate the expression.
    switch (tokens[main_op].type) {
      case TK_PLUS: 
        ret = (int)val1 + (int)val2;
        break;
      case TK_MINUS:
        ret = (int)val1 - (int)val2;
        break;
      case TK_MUL:
        ret = (int)val1 * (int)val2;
        break;
      case TK_DIV: {
        if(val2 == 0) {
          // panic("Divided by 0!");
          ret = 0;
        }
        else{
          ret = (int)val1 / (int)val2;
        }
        break;
      }
      case TK_NEG:
        ret = -(int)val2;
        break;
      case TK_EQ:
        ret = (int)val1 == (int)val2;
        break;
      case TK_NEQ:
        ret = (int)val1 != (int)val2;
        break;
      case TK_DEREF: 
        ret = paddr_read(val2, 4);
        break;
      default:
        panic("[expr eval] Bad operators!");
        break;
    }
    return ret;
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  *success = true;
  return eval(0, nr_token - 1);
}

static char buf[65536] = {0};

void test_all() {
  FILE *fp = fopen("/home/bluespace/projects/os/nemu/nemu/tests/input.txt", "r");
  assert(fp != NULL);
  const size_t ret_code = fread(buf, 1, 65536, fp);
  assert(ret_code > 0);
  fclose(fp);
}

void test_once() {
  memcpy(buf, "4153280252: ((82)) - ( 68 *  63/ 9 -43  )*( (( 48+1)) )*( 21*(6)*(( 53)))\n", 74);
}

void test_expr() {
  // test_once();
  test_all();
  char *string = buf;
  while (string[0]) {
    char *expr_line = strtok(string, "\n");
    string += strlen(expr_line) + 1;
    char *result_str = strtok(expr_line, ":");
    int ref_result = atoi(result_str);
    expr_line += strlen(result_str) + 1;
    bool success = false;
    int our_result = expr(expr_line, &success);
    printf("[expr test]: expr: %s, our result: %d, ref result: %d, correct: %s\n", expr_line, our_result, ref_result, our_result == ref_result ? "yes" : "no");
  }
}