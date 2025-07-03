#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include "memory.h"
#include <assert.h>
#include "sim.h"
#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))

extern NPCState npc_state;
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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
  line_read = readline("(npc) ");

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
		if (*str<48 || *str>57) {// 
			return -1;  // 字符串不是大于0的整数的整数
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
  npc_state.state = NPC_QUITE;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

//static int cmd_d(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NPC", cmd_q },

  /* TODO: Add more commands */
  {
		"si",
		"Step into N instruction and halt.if N is not given, the defaut is 1",
		cmd_si
	},
  {
    "info",
    "print register",
    cmd_info
  },
  {
    "x",
    "print memory",
    cmd_x,
  },
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
	}
  return 0;
}

static int cmd_info(char *args) {
  char *argument = strtok(args, " ");
  if (argument == NULL) {
    printf("info r  -- print register\n");
    return 0;
  }
  if (*argument == 'r') {
    reg_display();
    return 0;
  }
  printf("invaild argument.\n");
  return 0;
}


static int cmd_x(char *args) {

  char *number = strtok(args, " ");
  char *expression = strtok(NULL, " ");
  if (expression == NULL) {
    printf("x N expression  -- print memort\n");
    return 0;
  }

  int N = string_to_number(number);
  assert(N<1000);
  if (N == -1) {
    printf("after x is not a vaild number\n");
    return 0;
  }

  uint32_t addr;

  sscanf(expression, "%x", &addr); 
  if (addr < 0x80000000 || addr - 0x80000000 > MEM_MAX || (0x80000000+MEM_MAX) - addr < N) {
    printf("addr:%x N:%d, invaild address or N is too big\n", addr, N);
    return 0;
  }

  for (int i = 0; i < N; i++) {
     printf("0x%8x: 0x%08x\n", addr+i*4, pmem_read(addr+i*4));
  }
  return 0;
}


void sdb_mainloop() {
  for (char *str; (str = rl_gets()) != NULL; ) {         // 读取用户输入
	  char *str_end = str + strlen(str);                   // 计算输入字符串的结尾位置

    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }


    int i;
    for (i = 0; i < NR_CMD; i ++) {
	      if (strcmp(cmd, cmd_table[i].name) == 0) {
        /*
         * 返回值小于0则退出循环
         * 也就是说命令的返回值用来控制是否退出循环
         * */
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

