#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case -1: 
        ev.event = EVENT_YIELD; 
        c->mepc += 4;
        break;

      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

// 传入处理回调函数handler
bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  // 可能优化的原因是编译器认为 csrw mtvec 只是写入硬件寄存器，不影响程序内存状态
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));  // 将__am_asm_trap这个汇编定义的标签地址值放入寄存器中

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
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
