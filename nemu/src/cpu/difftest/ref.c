/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>
#include <cpu/decode.h>

__EXPORT void difftest_memcpy(uint8_t *addr, uint8_t *buf, size_t n) {
  size_t i;
  for(i = 0; i < n; i++) {
    addr[i] = buf[i];
  }
}

__EXPORT void difftest_regcpy(CPU_state *ref_r, CPU_state *dut_r) {
  for(int i = 0; i < 32; i++) {
    ref_r->gpr[i] = ref_r->gpr[i];
  }
  ref_r->pc = ref_r->pc;
}

int isa_exec_once(Decode *s);
extern CPU_state cpu;
__EXPORT void difftest_exec() {
  
  Decode s;
  s.pc = cpu.pc;
  s.snpc = cpu.pc;
  isa_exec_once(&s);
  cpu.pc = s.dnpc;
}

__EXPORT void difftest_get_ref_r(CPU_state *ref_r) {
  for(int i = 0; i < 32; i++) {
    ref_r->gpr[i] = cpu.gpr[i];
  }
  ref_r->pc = cpu.pc;
}

__EXPORT void difftest_raise_intr(word_t NO) {
  assert(0);
}

__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();
  /* Perform ISA dependent initialization. */
  init_isa();
}

