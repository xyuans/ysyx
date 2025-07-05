#include "common.h"

void sdb_mainloop();

int main(int argc, char *argv[]) {
  sim_init();
  // 将程序读入内存，并打印前10个字节的值
  mem_init(argv[1]);
  reset(2);
  sdb_mainloop();
  sim_exit();

  return 0;
}

