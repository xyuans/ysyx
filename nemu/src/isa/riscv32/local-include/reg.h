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

#ifndef __RISCV_REG_H__
#define __RISCV_REG_H__

#include <common.h>
#include <isa.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < MUXDEF(CONFIG_RVE, 16, 32)));
  return idx;
}

static inline word_t *csr_by_idx(int idx) {
  word_t *p_csr;
  switch (idx) {
    case 0x300: p_csr = &cpu.mstatus; break;
    case 0x305: p_csr = &cpu.mtvec;   break;
    case 0x341: p_csr = &cpu.mepc;    break;
    case 0x342: p_csr = &cpu.mcause;  break;
    default: assert(0);
  }
  return p_csr;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])
#define p_csr(idx) (csr_by_idx(idx))

static inline const char* reg_name(int idx) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

#endif
