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
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";



static void gen_rand_expr() {
  buf[0] = '\0';
  int random_num = rand() % 3
  switch (random_num) {
    case 0: 
  }
}

int main(int argc, char *argv[]) {
  /*
   * time_t time(time_t *timer);
   * 获取当前的系统时间
   * 参数timer的值可以为NULL,time()函数仍然会将当前的系统时间作为返回值。
   * */
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    /*
     * int sprintf(char *buffer,const char *format[,argument...]); 
     * sprintf() 函数用于将格式化的输出存储在字符数组中，而不是写入标准输出设备
     * */
    sprintf(code_buf, code_format, buf);
    
    /*
     * 1. /tmp目录存放临时文 件，10天后会被清理
     * 2. FILE *fopen(char *filename, char *mode);
     *    "w"模式写入文件，文件不存在则会创建，存在则会覆盖之前的。
     * */
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);
    
    /*
     * int system(const char *command);
     * 当 command 不为 NULL 时，函数成功执行命令返回 0，否则返回 -1 或其他非零值
     * 用来执行shell命令
     * */
    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
