#include "common.h"
#include <dlfcn.h>
#include <assert.h>
#include <stdio.h>

static void (*ref_difftest_memcpy)(uint8_t *src, size_t n);
static void (*ref_difftest_regcpy)(CPU_state *dut_r);
static void (*ref_difftest_exec)();
static void (*get_ref_r)(CPU_state* ref_r);

extern void get_dut_r(CPU_state* dut_r);
extern const char *regs[];
void init_difftest(char *ref_so_file, long img_size) {
  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = (void (*)(uint8_t*, size_t))dlsym(handle, "difftest_memcpy");
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = (void (*)(CPU_state*))dlsym(handle, "difftest_regcpy");
  assert(ref_difftest_regcpy);

  ref_difftest_exec = (void (*)())dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);


  void (*ref_difftest_init)() = (void (*)())dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  get_ref_r = (void (*)(CPU_state*))dlsym(handle, "difftest_get_ref_r");
  assert(get_ref_r);
  ref_difftest_init();
  ref_difftest_memcpy(guest_to_host(0x80000000), img_size);
  CPU_state dut_r;
  get_dut_r(&dut_r);
  ref_difftest_regcpy(&dut_r);
  printf("difftest is on\n");
}

static bool checkregs(CPU_state *ref_r, CPU_state *dut_r) {
  if (ref_r->pc != dut_r->pc) {
    return -1;
  }
  for (int i = 0; i < 32; i++) {
    if(ref_r->gpr[i] != dut_r->gpr[i])
    return i;
  }
  
  return -2;
}

bool difftest_step() {
  // nemu执行一步
  ref_difftest_exec();
  CPU_state dut_r;
  CPU_state ref_r;
  get_dut_r(&dut_r);
  get_ref_r(&ref_r);

  int index = checkregs(&ref_r, &dut_r); 
  if(!checkregs(&ref_r, &dut_r)) {
    printf("can not catch up with ref\nerror reg: %d\nnpc:\n", index);
    printf(" -1%-10s%-#15x%-15d\n", "pc", dut_r.pc, dut_r.pc);
    for (int i=0; i<32; i++) {
      printf("%2d%-10s%-#15x%-15d\n", i, regs[i], dut_r.gpr[i], dut_r.gpr[i]);
    }
    printf("--------\nnemu:\n% -1%-10s%-#15x%-15d\n", "pc", ref_r.pc, ref_r.pc);
    for (int i=0; i<32; i++) {
      printf("%2d%-10s%-#15x%-15d\n", i, regs[i], ref_r.gpr[i], ref_r.gpr[i]);
    }
    return false;
  }
  return true;
}
