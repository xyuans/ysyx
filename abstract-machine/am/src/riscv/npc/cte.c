#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    #ifdef __riscv_e
    int call_id = c->gpr[15];
    #else
    int call_id = c->gpr[17];
    #endif 
    switch (call_id) {  // c->gpr[17] is a7
      case -1: 
        ev.event = EVENT_YIELD;
        c->mepc += 4;
        break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    //printf("after c->mepc:%x\n", c->mepc);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *cxt = (Context *)((uintptr_t)kstack.end - sizeof(Context));  // 前向32个通用寄存器和四个特殊的寄存器
  // 初始条件下gpr的上下文信息。
  cxt->gpr[2] = (uint32_t)cxt;
  cxt->gpr[10] = (uintptr_t)arg;
  cxt->mstatus = 0x1800;
  cxt->mepc = (uintptr_t)entry;
  printf("cxt is:%x\n", cxt);
  return cxt;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
