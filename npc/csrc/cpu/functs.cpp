#include "functs.h"
#include <stdio.h>

int npc_status = 0;

extern "C" uint32_t ebreak() {
  npc_status = STOP;
  printf("happen\n");
  return 0;
}

