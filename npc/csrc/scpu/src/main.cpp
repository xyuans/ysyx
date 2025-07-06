#include "common.h"

void sdb_mainloop();

static char *img_file = NULL;
static char *elf_file = NULL;
bool log_write = false;
bool trace_on = false;

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"log"      , required_argument, NULL, 'l'},
    {"help"     , no_argument      , NULL, 'h'},
    {"trace"    , required_argument, NULL, 't'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:f:", table, NULL)) != -1) {
    switch (o) {
      case 'l': log_write = true; break;
      case 't': elf_file = optarg;log_write = true; break;  // 初始化
      case 1:   img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-l,                     output log to trace-log.txt\n");
        printf("\t-t,--trace=ELF_FIEL    open trace function\n"));
        printf("\n");
        exit(0);
    }
  }
  return 0;
}


int main(int argc, char *argv[]) {
  sim_init();
  // 将程序读入内存，并打印前10个字节的值
  mem_init(img_file);
  reset(2);
  if (trace_on) {
    trace_init(elf_file);
  }
  sdb_mainloop();
  sim_exit();
  if (trace_on) {
    trace_exit();
  }
  return 0;
}

