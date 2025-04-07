/*
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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

word_t expr(char *e, bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  /* Start engine. */
  //engine_start();

  //return is_exit_status_bad();
  printf("%s,%s\n", argv[0], argv[1]);
  FILE *fp = fopen("test.log", "r");
  assert(fp);

  char *result;
  char *expression;
  char line[256];
  bool success = true;

  int i = 1;
  while(fgets(line, 256, fp)){
    success = true;
    result = strtok(line, " ");
    expression = strtok(NULL, "\n");

    word_t caculate = expr(expression, &success);
    int a;
    printf("%d expression:%s, result:%s, caculate:%u\n",i, expression, result, caculate);
  
    sscanf(result, "%d", &a);
    if (a != caculate) {
      fprintf(stderr, "%d expression is error\n", i);
    }
    i++;
  }
}
