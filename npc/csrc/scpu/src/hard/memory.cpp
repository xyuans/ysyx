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


uint32_t access_addr;
extern "C" uint32_t pmem_read(uint32_t addr) {
  if (addr == 0) return 0;
  uint32_t index = addr - 0x80000000;
  if (index < 0 || index > MEM_MAX) {
    printf("pmem_read, pc: %08x, beyond MEM_MAX\n", addr);
    exit(-1);
  }
  
  access_addr = addr;  // 记录内存访问地址，以提供mtrace
  return *(uint32_t *)(mem+index);
}

// 总是往地址为`waddr & ~0x3u`的4字节按写掩码`wmask`写入`wdata`
// `wmask`中每比特表示`wdata`中1个字节的掩码,
// 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变
extern "C" void pmem_write(uint32_t waddr, uint32_t wdata, int len) {
  uint32_t index = waddr - 0x80000000;
  if (index < 0 || index > MEM_MAX) {
    printf("pmem_write, pc: %08x, beyond MEM_MAX\n", waddr);
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

  access_addr = waddr;  // 记录内存访问地址，以供mtrace使用
}


uint8_t *guest_to_host(uint32_t addr) {
  uint32_t index = addr - 0x80000000;
  return &mem[index];
}
