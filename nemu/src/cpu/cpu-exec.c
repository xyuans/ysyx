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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>
#include <string.h>
#include <monitor/ftrace.h>
/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10


extern SymList symlist;  // ftrace.c

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

void device_update();

int check_wps();  // 监视点的值发生变化时返回1,否则返回0；

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));

#ifdef CONFIG_TRACE
  if (symlist.exist == true) {
    // 函数调用栈
    typedef struct Fun_Stat {
      int stat[64];
      int p;
    } Fun_Stat;
    static Fun_Stat fun_stat = {.p = -1};

    //00001_000_00000_1100111   0x08067 为 ret指令
    if ((_this->isa.inst & 0xff07f) == 0x8067) {
      uint32_t location = 0;
      int index = 0;
      for (int i = fun_stat.p; i >= 0 ; i--) {
        // 返回位置在dnpc地址之前最近的一个函数
        uint32_t addr = symlist.list[fun_stat.stat[i]].addr;
        if (_this->dnpc > addr && addr > location) {
          location = addr;
          index = i;
        }
      }
      fun_stat.p = index;
      printf("%2d ret  [#addr:%x] #%s\n", index, _this->dnpc, symlist.list[fun_stat.stat[index]].name);
    }
    //jal和jalr
    else if ((_this->isa.inst & 0x7f) == 0x6f || (_this->isa.inst & 0x707f) == 0x67){   // 0x6f=11011_11
      for (int i = 0; i < symlist.count; i++) {
        if (_this->dnpc == symlist.list[i].addr) {
          fun_stat.p++;
          if (fun_stat.p > 31 || fun_stat.p < 0) {
            printf("function statck is overflower.\n");
            exit(-1);
          }
          fun_stat.stat[fun_stat.p] = i;
          printf("%2d call [@addr:%x] @%s\n", fun_stat.p, \
                symlist.list[i].addr, symlist.list[i].name);
        }
      }
    }
  }
  // iringbuf
  if (nemu_state.halt_ret != 0) {
    int cur = _this->iringbuf.cur + 1;
    int i;
    for (i = 0; i < 15; i++) {
      printf("   %s\n", _this->iringbuf.buf[cur]);
      cur = (cur + 1) % 16;
    }
    printf("-->%s\n", _this->iringbuf.buf[cur]);
  }
#endif

#ifdef CONFIG_WATCHPOINT
  if (check_wps()) {
    nemu_state.state = NEMU_STOP;
  }
#endif
}
static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s);
  cpu.pc = s->dnpc;

#ifdef CONFIG_TRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst;
#ifdef CONFIG_ISA_x86
  for (i = 0; i < ilen; i ++) {
#else
  for (i = ilen - 1; i >= 0; i --) {
#endif
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst, ilen);

  // mtrace 
  int cur = (s->iringbuf.cur + 1) % 16;
  s->iringbuf.cur = cur;
  strncpy(s->iringbuf.buf[cur], s->logbuf, sizeof(s->iringbuf.buf[cur]) );
#endif
}

static void execute(uint64_t n) {
  Decode s;
  for (;n > 0; n --) {
    exec_once(&s, cpu.pc);
    g_nr_guest_inst ++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (nemu_state.state) {    //enum { NEMU_RUNNING, NEMU_STOP, NEMU_END, NEMU_ABORT, NEMU_QUIT };
    case NEMU_END: case NEMU_ABORT: case NEMU_QUIT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  execute(n);
  
  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      // 释放symlist.list
      #ifdef CONFIG_TRACE
        free(symlist.list);
        symlist.list = NULL;
      #endif

      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);
      // fall through
    case NEMU_QUIT: statistic();
  }
}
