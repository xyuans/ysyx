#include "verilated.h"  // 这是 Verilator 的主头文件

#include "Vtop.h"
#include "verilated_fst_c.h"

#include <stdint.h>
#include <stdio.h>

#include "functs.h"

void mem_init(char *filename);
uint32_t pmem_read(uint32_t addr);
extern int npc_status;

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


int main(int argc, char *argv[]) {
  sim_init();
  mem_init(argv[1]);
  reset(2);
  int i = 15;
  while (i) {
    top->inst = pmem_read(top->pc);
    printf("top->inst:%08x, top->pc:%08x\n", top->inst, top->pc);
    step_and_dump_wave();
    if (npc_status == STOP || top->inst==1) break;
    i--;
  }
  sim_exit();

  return 0;
}

