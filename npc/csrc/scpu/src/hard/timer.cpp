#include <sys/time.h>
#include <time.h>
#include <stdint.h>
static uint32_t timer_reg[2];
uint64_t boot_time;
void set_bool_time() {
  struct timeval now;
  gettimeofday(&now, NULL);
  uint64_t us = now.tv_sec * 1000000 + now.tv_usec;
  boot_time = us;
}

void get_time() {
  if (boot_time == 0) set_bool_time();
  struct timeval now;
  gettimeofday(&now, NULL);
  uint64_t us = now.tv_sec * 1000000 + now.tv_usec;
  *(uint64_t *)timer_reg = us - boot_time;
}

uint32_t low_timer_reg () {
  return timer_reg[0];
}
uint32_t high_timer_reg() {
  return timer_reg[1];
}
