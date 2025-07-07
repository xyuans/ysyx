#include "common.h"
static void (*ref_difftest_memcpy)(uint8_t *src, size_t n);
static void (*ref_difftest_regcpy)(CPU_state *dut_r);
static void (*get_ref_r)(CPU_state* ref_r);


void init_difftest(char *ref_so_file, long img_size) {
  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = dlsym(handle, "difftest_memcpy");
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = dlsym(handle, "difftest_regcpy");
  assert(ref_difftest_regcpy);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);


  void (*ref_difftest_init)(int) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  get_ref_r = dlsym(handle, "difftest_get_ref_r");
  assert(get_ref_r);

  ref_difftest_init();
  ref_difftest_memcpy(guest_to_host(0x80000000), img_size);
  CPU_state dut_r;
  get_dut_r(&dut_r);
  ref_difftest_regcpy(&dut_r);
}

static bool checkregs(CPU_state *ref_r, CPU_state *dut_r) {
  for (int i = 0; i < 32; i++) {
    if(ref_r->gpr[i] != dut_r->gpr[i])
    return false;
  }
  if (ref_r->pc == cpu.pc) return true;
  return false;
}

bool difftest_step(uint32_t pc, uint32_t npc) {
  // nemu执行一步
  ref_difftest_exec();
  CPU_state dur_r;
  CPU_state ref_r;
  get_dut_r(&dur_r);
  get_ref_r(&ref_r);

  return checkregs(&ref_r, &dut_r));
}
