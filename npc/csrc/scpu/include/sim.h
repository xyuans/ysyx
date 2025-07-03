#ifndef __SIM_H__
#define __SIM_H__
#include <stdint.h>

enum {
  NPC_RUNNING,
  NPC_STOP,
  NPC_QUITE
};
extern NPCState npc_state;
void sim_init();
void step_and_dump_wave();
void sim_exit();
void reset(int n);
void exec_once();
void cpu_exec(uint64_t n);
void reg_display();
#endif
