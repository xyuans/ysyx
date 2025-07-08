#ifndef __TRACE_H__
#define __TRACE_H__
  
typedef struct TraceDiffState {
  bool wtrace;
  bool itrace;
  bool mtrace;
  bool ftrace;
} TraceDiffState;

void iringbuf_print();
void logbuf_print();
void trace_init(char *elf);
void trace_exit();
void trace();

#endif
