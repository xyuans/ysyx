#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>

#define MEM_MAX 65536  // 2^16

void mem_init(char* filename);
uint32_t pmem_read(uint32_t addr);
#endif
