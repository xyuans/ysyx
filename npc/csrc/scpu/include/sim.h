#ifndef __SIM_H__
#define __SIM_H__
#include <stdint.h>
void sim_init();
void step_and_dump_wave();
void sim_exit();
void reset(int n);
void exec_once();
void cpu_exec(uint64_t n);
void reg_display();
#endif
