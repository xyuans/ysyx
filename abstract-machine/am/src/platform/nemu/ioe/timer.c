#include <am.h>
#include <nemu.h>

static uint64_t boot_time;

static uint64_t read_time() { 
  // volatile 关键字告诉编译器，该内存位置的内容可能被硬件或外部事件改变，因此每次访问都必须严格执行，不能优化
  // 否则可能因为代码中没有改过这个地址的值而导致编译器会再第二次读取时读取之前加载到寄存器的值。
  uint32_t hi = *(volatile uint32_t *)(RTC_ADDR + 4);
  uint32_t lo = *(volatile uint32_t *)(RTC_ADDR + 0);
  uint64_t time = ((uint64_t)hi << 32) | lo;
  return time;
}

void __am_timer_init() {
  boot_time = read_time();
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = read_time() - boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
