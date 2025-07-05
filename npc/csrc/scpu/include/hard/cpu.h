#ifndef __SIM_H__
#define __SIM_H__
#include <stdint.h>

enum {
  NPC_RUNNING,
  NPC_STOP,
  NPC_QUITE
};
typedef struct {
  int state;             // 状态标识（如运行、暂停、异常等）
  uint32_t halt_pc;       // 停止时的程序计数器（PC）值
  uint32_t halt_ret;     // 停止时的返回值或错误码
} NPCState;

void sim_init(char* arg);
void step_and_dump_wave();
void sim_exit();
void reset(int n);
void exec_once();
void cpu_exec(uint64_t n);
void reg_display();
#endif
