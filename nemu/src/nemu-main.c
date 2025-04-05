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
// #ifdef CONFIG_TARGET_AM
//   am_init_monitor();
// #else
//   init_monitor(argc, argv);
// #endif
//
//   /* Start engine. */
//   engine_start();
//
//   return is_exit_status_bad();
printf("%s,%s\n", argv[0], argv[1]);
 char *f = argv[1];
  FILE *fp = fopen(f, "r");
  assert(fp);
  char line[256];

  bool success;
 success = true;

//  uint32_t correct;

  while (fgets(line, 256, fp)) {
  char *result = strtok(line, " ");
  char *exr = strtok(NULL, " ");

  uint32_t value = expr("3+2", &success);
  //sscanf(result, "%u", &correct);
   // printf("expression: %s, correct: %u, but we get:  %u", exr, correct, value);
  printf("%d, exr:%s, result:%s", value, exr, result);
  }
}
