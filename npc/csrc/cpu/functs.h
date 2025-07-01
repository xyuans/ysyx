#ifndef __FUNCTS_H__
#define __FUNCTS_H__
#include <stdint.h>
enum {
  RUN=0,
  STOP,
};
extern "C" uint32_t ebreak();
#endif
