/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
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
#include <string.h>
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
/*
 * 1. enmu内部的标识符，通常都使用大写
 * 2. 默认从0值开始递增0，规则是这样的:
 *      enum{
 *        A,      // 0
 *        B = 5,  // 5  
 *        C,      // 6
 *        D = 3,  // 3
 *        E       // 4
 *      }
 * 3. 这里的用法是声明了一组常量。
 * 4. ASCII表一共有128个字符
 *  */
enum {
  TK_NOTYPE = 256,
  TK_EQ = 128,

  /* TODO: Add more token types */
  TK_NUM
};

/*
 * 1. +为量词，表示匹配前一个元素一次或多次
 * 2. \为转义字符，在C语言中，\本身需要转义，所以\\+表示字符串\+，\+
 *    在正则表达式中表示真实字符+。
 * */
static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +",    TK_NOTYPE},    // spaces
  {"==",    TK_EQ},        // equal
  {"[0-9]+",  TK_NUM},       // number
  {"\\+",   '+'},          // plus
  {"-",     '-'},
  {"\\*",   '*'},
  {"/",     '/'},
  {"\\(",   '('},
  {"\\)",   ')'},
};

#define NR_REGEX ARRLEN(rules)

/*
 * regex_t结构体存储编译后的正则表达式模式，以便高效重复使用。
 * */
static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  /*
   * 1. regcomp()函数将字符串形式的正则表达式(如 "[0-9]+")编译成底层可执行的模式。
   *      &re[i]：目标存储位置，类型为 regex_t 的指针。
   *      rules[i].regex：源字符串，即预定义的正则表达式（如rules结构体的regex字段）
   *      REG_EXTENDED：标志位，表示使用扩展正则语法（支持更丰富的语法，如 +、?、|）
   * */
  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

/*
 * 1. 编译器在开启优化时，会自动删除“未被使用”的一些静态变量
 * 2. __attribute__((used))的作用是强制编译器保留某些符号
 * */
static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static int make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;     // 指示已经被识别出的token数目

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    /* 1. 用所有预定义的正则表达式规则（re 数组）尝试匹配当前位置的字符串
     * 2. int regexec(
     *       const regex_t *preg,         // 编译后的正则表达式
     *       const char *string,          // 目标字符串
     *       size_t nmatch,               // pmatch数组大小
     *       regmatch_t pmatch[],         // 存储匹配位置的结构体数组
     *       int eflags                   // 执行标志
     *     );
     *     该函数匹配成功会返回0
     * 3. typedef struct {
     *          regoff_t rm_so;           // 匹配的起始偏移（从 string 开头计算）
     *          regoff_t rm_eo;           // 匹配的结束偏移（不包含该字符）
     *      } regmatch_t;
     * */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
      
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);


        /* TODO: Now a new token is recognized with rules[i]. Add codes
          * to record the token in the array `tokens'. For certain types
          * of tokens, some extra actions should be performed.
          */
        
        /*
          * 1. void *memcpy(void *dest, const void *src, size_t n);
          * */
        assert(substr_len <= 32);
        tokens[nr_token].type = rules[i].token_type;
        memcpy(tokens[nr_token].str, e + position, substr_len);
        tokens[nr_token].str[substr_len] = '\0';
        // switch (rules[i].token_type) {
        //   case TK_NUM:
        //     assert(substr_len <= 32);
        //     tokens[nr_token].type = TK_NUM;
        //     assert()
        //   default: TODO();
        // }
        nr_token++;
        position += substr_len;
        break;   // 阻止了接下来的匹配，让i != NR_REGEX
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return 0;
    }
  }
  
  return nr_token;
}


static bool match_parentheses(const char *str) {
  int count = 0;
  for (; *str != '\0'; str++) {
    if (*str == '(') { count++; }
    else if(*str == ')') {
      if (count == 0) { return false; }
      count--;
    }
  }
  return count == 0;
}

static bool check_parentheses(int p,int q) {
  if ((tokens[p].type == '(') && (tokens[q].type == ')')) {
    int count = 0;
    p++;
    for (;p != q; p++) {
      if (tokens[p].type == '(') { count++; }
      else if (tokens[p].type == ')') {
        if (count == 0) { return false; }
        count--;
      }
    }
    return count == 0;
  }
  return false;
}

static int get_op(int p, int q) {
  int op1 = -1;
  int op2 = -1;
  for (;p != q; p++) {
    if (tokens[p].type == '+' || tokens[p].type == '-') {
      op1 = p;
    }
    else if (tokens[p].type == '*' || tokens[p].type == '/') {
      op2 =p;
    }
    else if (tokens[p].type == '(') {
      do {
        p++;
      } while(tokens[p].type == ')');
    }
  }
  return op1>0 ? op1 : op2;
}

static uint32_t eval(int p, int q) {
  assert(p <= q);
  if (p == q) {
    int value;
    char *str = tokens[p].str;
    for (;*str != '\0'; str++) {
      if (*str < 47 || *str > 58) {
        printf("bad expression\n");
        exit(1);
      }
    }
    sscanf(tokens[p].str, "%d", &value);
    return value;
  }
  else if (check_parentheses(p, q)) {
    return eval(p + 1, q - 1);
  }
  else {
    int op = get_op(p, q);
    int val1 = eval(p, op - 1);
    int val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/':
        if (val2 == 0) {
          printf("bad expression\n");
          exit(1);
        }
        return val1 / val2;
      default: assert(0);
    }    
  }
}

word_t expr(char *e, bool *success) {
  int len = make_token(e);
  if (len == 0) {
    *success = false;
    return 0;
  }
 


  /* TODO: Insert codes to evaluate the expression. */
  if (!match_parentheses(e)) {
      printf("bad expression:parentheses don't match\n");
      exit(1);
  }
 
  int p = 0;  // 子字符串起始位置
  int q = len - 1;  // 子字符串终止位置
  
  return eval(p, q);
}
