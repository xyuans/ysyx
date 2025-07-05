#include "verilated.h"  // 这是 Verilator 的主头文件
#include "Vtop.h"
#include "verilated_fst_c.h"
#include "Vtop___024root.h"  // 为了访问内部信号
#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>

extern trace_diff_state;

NPCState npc_state;
uint32_t cur_pc; 
uint32_t cur_inst;

// 为支持打印寄存器
static const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
static uint32_t steps;
static bool print_step = false;

static VerilatedContext* contextp = NULL;
static VerilatedFstC* tfp = NULL;
static Vtop *top = NULL;


// 仿真任何模块都可以使用这个代码块
void sim_init()
{
	contextp = new VerilatedContext;
	top = new Vtop{contextp};
	tfp = new VerilatedFstC;

  // 波形写入初始化
	contextp->traceEverOn(true); 
	top->trace(tfp, 1);
	tfp->open("cpu_wave.fst");
  print_wave = true;

  trace_init();
}

// 区别与有无clk
void step_and_dump_wave()
{ 
  top->clk = 0;top->eval();contextp->timeInc(1);  // 即使不写波形，仍需调用contextp->timeInc(1)
  if (trace_diff_state.wtrace == true) tfp->dump(contextp->time());

  top->clk = 1;top->eval();contextp->timeInc(1);
  if (trace_diff_state.wtrace == true) tfp->dump(contextp->time());
}


void sim_exit()
{
	tfp->close();
  top->final();
  // Destroy model
  delete top;

  trace_exit();
}


void reset(int n) {
  top->rst = 1;
  while (n-- > 0) step_and_dump_wave();
  top->rst = 0; step_and_dump_wave();
}


extern "C" void ebreak() {
  npc_state.state = NPC_STOP;
  npc_state.halt_pc = top->pc;
  npc_state.halt_ret = top->rootp->top__DOT__rf__DOT__regs[10];
  printf("ebreak happen\n");
}

// 更新cur_pc, cur_inst, steps, 退进一步仿真
void exec_once() {
  cur_pc = top->pc;  // 进行一步仿真之后,pc值会更新为下一条指令位置。
  top->inst = pmem_read(top->pc);
  cur_inst = top->inst;
  
  steps++;
  step_and_dump_wave();
}

void cpu_exec(uint64_t n) {
  // 检查运行状态
  switch (npc_state.state) {
    case NPC_STOP:
      printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
    case NPC_QUITE:
      return;
    default: npc_state.state = NPC_RUNNING;
  }
  
  for (int i = 0; i < n; i++) {
    print_step = (n < 11);
    exec_once();
    // 把追踪信息写入log文件,diff test
    trace_diff();
    if (print_step) logbuf_print();

    // 执行完一步就检查一下运行状态
    if (npc_state.state == NPC_STOP) {
      printf("final pc is: %x, steps is: %d\n", npc_state.halt_pc, steps);
      if (npc_state.halt_ret == 0) {
        printf("HIT GOOD TRAP\n");
      }
      else {
        iringbuf_write();
        printf("HIT BAD TRAP\n");
      }
      break;
    }
  }
}

void reg_display() {
  printf(" %-10s%-#15x%-15d\n", "pc", cur_pc, cur_pc);
  for (int i=0; i<32; i++) {
    printf(" %-10s%-#15x%-15d\n", regs[i], top->rootp->top__DOT__rf__DOT__regs[i],
           top->rootp->top__DOT__rf__DOT__regs[i]);
  }
}


