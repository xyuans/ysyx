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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
 /*
 * 1. 静态局部变量（static修饰）在程序启动时初始化，生命周期持续到程序结束，即使函数执行结束，变量依然存在。
 *    静态局部变量的作用域仅限定义它的函数内部，其他函数无法直接访问它。  
 * */
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }
  
  /*
   * 1. readline用于读取一行输入。可以指定一个prompt，显示在输入前（nemu）
	 *     返回一个指向输入内容的指针。
   * 2. add_history(const char *line) 用于将字符串添加到历史记录的命令，
   *    方便用户上下左右浏览之前命令
   * */
  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

/*
 * 1. 问题：1. 函数接受的输入一定是字符串吗？
 *          2. 用户输入不一定是大于零的整数，要进行判断
 *          3. 整数过大怎么办？这个情况也要处理
 * */
static int string_to_number(const char *str) {
	unsigned int number = 0;
	for(;*str != '\0';str++) {
		if (*str<49 || *str>57) {
			return -1;  // 字符串不是大于1的整数
		};
		number = number * 10 + (*str) - 48; 
	}
	return number;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {
		"si",
		"Step into N instruction and halt.if N is not given, the defaut is 1",
		cmd_si
	}
};
 
#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}



static int cmd_si(char *args) {
	char *arg = strtok(args, " ");
	if (arg == NULL) {
		cpu_exec(1);
	}
	else {
		int steps = string_to_number(arg);
		if (steps == -1) {
			printf("si: expect a number greater than 0\n");
			return 0;
		}
		cpu_exec(steps);
		return 0;
	}
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {             // 检查是否处于批处理模式
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {         // 读取用户输入
	/*
	 * strlen()函数用来计算字符串长度，不包括/0.
	 * str_end最终指向字符串中存储/0的这个字节
	 * */
    char *str_end = str + strlen(str);                   // 计算输入字符串的结尾位置

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
			/*
			 * 1. int strncmp(const char *s1, const char *s2, size_t n);
			 * 2. 如果两个字符串相同则会返回0.
			 * */
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
