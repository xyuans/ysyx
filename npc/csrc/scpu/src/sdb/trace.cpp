#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sdb/trace.h"
typedef struct RingBuf {
  int cur;
  char buf[16][128];
} RingBuf;

static RingBuf iringbuf;

TraceDiffState trace_diff_state = {false, false, false, false, false};
static char logbuf[128];

extern uint32_t cur_pc;
extern uint32_t cur_inst;
static FILE *file = NULL;

void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);

static void iringbuf_write(char *str) {
  int cur = (iringbuf.cur + 1) % 16;
  iringbuf.cur = cur;
  strncpy(iringbuf.buf[cur], logbuf, 127);
}


void iringbuf_print() {
  int cur = iringbuf.cur + 1;
  int i;
  for (i = 0; i < 15; i++) {
    printf("   %s\n", iringbuf.buf[cur]);
    cur = (cur + 1) % 16;
  }
  printf("-->%s\n", iringbuf.buf[cur]);
}

void logbuf_print() {
  printf("%s", logbuf);
}

// 放在sim_init中
void trace_init() {
  file = fopen("trace-log.txt", "w");

  if (file == NULL) {
    printf("Failed to create file");

    exit(1);
  }
  return;
}


// 放在sim_exit()中
void trace_exit() {
  fclose(file);
}

// 放在cpu_exec中
void trace_diff() {
  // itrace一直开启，区别在于写不写入文件
  // 反汇编，将结果写入logbuf
  disassemble(logbuf, sizeof(logbuf), cur_pc, (uint8_t *)&cur_inst, 4);
  iringbuf_write(logbuf);
  if (trace_diff_state.itrace == true) fprintf(file, "%s", logbuf);
}
