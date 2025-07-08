#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "common.h"

static uint8_t mem[MEM_MAX];

uint64_t mem_init(char* filename) {
  FILE* file = fopen(filename, "rb"); // 以二进制读模式打开文件

  if (file == NULL) {
    perror("Failed to open file");
    exit(-1);
  }

  fseek(file, 0, SEEK_END); // 将文件指针移动到文件末尾

  long size = ftell(file);  // 获取文件指针当前位置，即文件大小
  
  if (size >= MEM_MAX) {
    printf("file size is %ld, greater then MEM_MAX\n", size);
    exit(-1);
  }
  
  rewind(file);

  long size_r = fread(mem, 1, size, file);
  //if (size_r != size) {
  //  printf("fread is error, size_r is:%ld, size is:%ld\n", size_r, size);
  //}
  fclose(file); // 关闭文件
  for (int i = 0; i < 10; i++) {
    printf("size:%ld, size_r:%ld, mem[%d]:%x\n",size, size_r, i, mem[i]);
  }

  return size;
}

extern "C" uint32_t pmem_read(uint32_t addr) {
  uint32_t index = addr - 0x80000000;
  if (index < 0 || index > MEM_MAX) {
    printf("pmem_read, pc: %08x, beyond MEM_MAX\n", addr);
    exit(-1);
  }
  return *(uint32_t *)(mem+index);
}


extern "C" void pmem_write(uint32_t waddr, uint32_t wdata, int len) {
  uint32_t index = addr - 0x80000000;
  if (index < 0 || index > MEM_MAX) {
    printf("pmem_write, pc: %08x, beyond MEM_MAX\n", wdddr);
    exit(1);
  }
  uint32_t *p_wdata = &wdata;
  switch (len) {
    case 1:
      *(uint8_t *) (mem+waddr) = *(uint8_t *) (p_wdata);
      break;
    case 2:
      *(uint16_t *)(mem+waddr) = *(uint16_t *)(p_wdata);
      break;
    case 4:
      *(uint32_t *)(mem+waddr) = *(uint32_t *)(p_wdata);
      break;
    default:
      printf("pmem_write,len is error\n");
      exit(1);
  }
}


uint8_t *guest_to_host(uint32_t addr) {
  uint32_t index = addr - 0x80000000;
  return &mem[index];
}
