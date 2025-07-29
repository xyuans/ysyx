#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

extern Area heap;
static uintptr_t next_free = 0;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()

  if (size == 0) return NULL;
  if (next_free == 0) next_free = (uintptr_t)heap.start;
  // 1. 对齐分配大小（向上取整到8字节）
  //size = (size + 7) & ~7;  
  
  // 2. 计算对齐后的起始地址
  uintptr_t aligned_addr = (next_free + 7) & ~7;
  
  // 3. 计算新地址并更新全局变量
  uintptr_t new_addr = aligned_addr + size;
  if (new_addr > (uintptr_t)heap.end) {
    halt(1);
    #if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
    panic("klib malloc error\n");
    #endif
  }
  next_free = new_addr;
  
  return (void *)aligned_addr;

}

void free(void *ptr) {
}

#endif
