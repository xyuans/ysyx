#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <memory.h>

uint8_t mem[MEM_MAX];

void mem_init(char* filename) {
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
}

uint32_t pmem_read(uint32_t addr) {
  uint32_t index = addr - 0x80000000;
  if (index < 0 || index > MEM_MAX) {
    printf("pc: %08x, beyond MEM_MAX.\n", addr);
    return 1;
  }
  printf ("index:%u, mem[index]: %x\n", index, mem[index]);
  return *(uint32_t *)(mem+index);
}


