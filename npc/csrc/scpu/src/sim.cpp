#include "verilated.h"  // 这是 Verilator 的主头文件
#include "Vtop.h"
#include "verilated_fst_c.h"
#include "sim.h"
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>



typedef struct {
  int state;             // 状态标识（如运行、暂停、异常等）
  uint32_t halt_pc;       // 停止时的程序计数器（PC）值
  uint32_t halt_ret;     // 停止时的返回值或错误码
} NPCState;

NPCState npc_state;

static bool g_print_step = false;

static uint32_t pre_pc; 

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

VerilatedContext* contextp = NULL;
VerilatedFstC* tfp = NULL;
Vtop *top = NULL;


// 仿真任何模块都可以使用这个代码块
void sim_init()
{
	contextp = new VerilatedContext;
	top = new Vtop{contextp};
	tfp = new VerilatedFstC;
	
	contextp->traceEverOn(true); 
	top->trace(tfp, 1);
	tfp->open("cpu_wave.fst");
}

// 区别与有无clk
void step_and_dump_wave()
{ 
  top->clk = 0;top->eval();contextp->timeInc(1);tfp->dump(contextp->time());
  top->clk = 1;top->eval();contextp->timeInc(1);tfp->dump(contextp->time());
}


void sim_exit()
{
	step_and_dump_wave();
	tfp->close();
}


void reset(int n) {
  top->rst = 1;
  while (n-- > 0) step_and_dump_wave();
  top->rst = 0; step_and_dump_wave();
}


extern "C" int ebreak() {
  npc_state.state = NPC_STOP;
  npc_state.halt_pc = top->pc;
  npc_state.halt_ret = top->rootp->__DOT__rf__DOT__regs[10];
  printf("ebreak happen\n");
}

void exec_once() {
  pre_pc = top->pc;  // 进行一步仿真之后,pc值会更新为下一条指令位置。
  top->inst = pmem_read(top->pc);
  step_and_dump_wave();
  if (g_print_step) {
    printf("top->inst:%08x, top->pc:%08x\n", top->inst, top->pc);
  }
}

void cpu_exec(uint64_t n) {
  g_print_step = (n < 10);

  switch (npc_state.state) {
    case NPC_STOP:
      printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
    case NPC_QUITE:
      return;
    default: npc_state.state = NPC_RUNNING;
  }
  
  for (int i = 0; i < n; i++) {
    exec_once();
    
    if (npc_state = STOP) {
      printf("final pc is: %x\n", npc_state.halt_pc);
      if (npc_state.halt_ret = 0) {
        printf("HIT GOOD TRAP\n");
      }
      else {
        printf("HIT BAD TRAP\n");
      }
      break;
    }
  }
}

void reg_display() {
  printf(" %-10s%-#15x%-15d\n", "pc", pre_pc, pre_pc);
  for (int i=0; i<32; i++) {
    printf(" %-10s%-#15x%-15d\n", regs[i], top->rootp->__DOT__rf__DOT__regs[i],
           top->rootp->__DOT__rf__DOT__regs[i]);
  }
}


