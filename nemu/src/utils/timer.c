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

#include <common.h>
#include MUXDEF(CONFIG_TIMER_GETTIMEOFDAY, <sys/time.h>, <time.h>)

IFDEF(CONFIG_TIMER_CLOCK_GETTIME,
    static_assert(CLOCKS_PER_SEC == 1000000, "CLOCKS_PER_SEC != 1000000"));
IFDEF(CONFIG_TIMER_CLOCK_GETTIME,
    static_assert(sizeof(clock_t) == 8, "sizeof(clock_t) != 8"));

static uint64_t boot_time = 0;

static uint64_t get_time_internal() {
#if defined(CONFIG_TARGET_AM)
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
#elif defined(CONFIG_TIMER_GETTIMEOFDAY)
  /* 1. timeval用于表示时间值的结构体
   * 2. #include <sys/time.h>
		struct timeval {
    	time_t      tv_sec;   // 秒（seconds）
    	suseconds_t tv_usec;  // 微秒（microseconds）
		};
   * 3. 通过 gettimeofday() 函数获取当前时间,第二个参数是时区（通常填 NULL）
   * 4. gettimeofday() 已被标记为过时，推荐使用 clock_gettime()（精度达纳秒级）
   */
  struct timeval now;
  gettimeofday(&now, NULL);
  uint64_t us = now.tv_sec * 1000000 + now.tv_usec;
#else
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC_COARSE, &now);
  uint64_t us = now.tv_sec * 1000000 + now.tv_nsec / 1000;
#endif
  return us;
}

uint64_t get_time() {
  if (boot_time == 0) boot_time = get_time_internal();
  uint64_t now = get_time_internal();
  return now - boot_time;
}

void init_rand() {
  /*
   * 1. void srand(unsigned int seed)
   * 2. 通过 seed 初始化随机数生成器的起点
   * 3. C 标准库内部定义了一个全局静态变量（通常名为 next），用于保存当前的随机种子。
   *    这个变量对程序员不可见，但可以通过 srand() 修改，通过 rand() 读取和更新。
   * */
  srand(get_time_internal());
}
