#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>

#define MEM_MAX 65536  // 2^16
uint64_t mem_init(char* filename);
extern "C" uint32_t pmem_read(uint32_t addr);
extern "C" void pmem_write(uint32_t waddr, uint32_t wdata, int len);
uint8_t *guest_to_host(uint32_t addr);
#endif
