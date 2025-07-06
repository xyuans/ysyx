#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ftrace用的有文件
#include <errno.h>
#include <elf.h>
#include <fcntl.h>  // 包含标志位定义 （O_RDONLY）
#include <sys/stat.h>  // 使用O_CREAT时需包含
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>

#include "sdb/trace.h"
typedef struct RingBuf {
  int cur;
  char buf[16][128];
} RingBuf;

static RingBuf iringbuf;

TraceDiffState trace_diff_state = {false, false, false, false, false};
static char logbuf[128];

extern uint32_t cur_pc;
extern uint32_t cur_inst;
extern uint32_t next_pc;
extern uint64_t steps;

static FILE *file = NULL;

extern "C" void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
extern "C" void init_disasm();
static void iringbuf_write(char *str) {
  int cur = (iringbuf.cur + 1) % 16;
  iringbuf.cur = cur;
  strncpy(iringbuf.buf[cur], logbuf, 127);
}


void iringbuf_print() {
  int cur = iringbuf.cur + 1;
  int i;
  for (i = 0; i < 15; i++) {
    printf("   %s\n", iringbuf.buf[cur]);
    cur = (cur + 1) % 16;
  }
  printf("-->%s\n", iringbuf.buf[cur]);
}

void logbuf_print() {
  printf("%s\n", logbuf);
}


typedef struct SymNode {
  uint32_t addr;
	char name[64];
} SymNode;

typedef struct SymList {
	bool exist;
  SymNode *list;
	int count;
} SymList;

SymList symlist = {.exist = false}; //__attribute__((section(".bss")));

void ftrace_init(char *filename) {
	int fd;
	struct stat st;
	uint8_t* mem = NULL;
	
	Elf32_Ehdr *ehdr = NULL;
	Elf32_Shdr *shdr = NULL;

	
	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(-1);
	}
	
	if(fstat(fd, &st) < 0) {
		perror("fstat");
		close(fd);
		exit(-1);
	}
	
	mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(mem == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(-1);
	}
	
	// 获取ELF头
	ehdr = (Elf32_Ehdr *) mem;
	if(mem[0] != 0x7f || strncmp((char *)&mem[1], "ELF", 3)) {
		fprintf(stderr, "%s is not an ELF file\n", filename);
		exit(-1);	
	}
	
	// 获取Section header
	shdr = (Elf32_Shdr *)&mem[ehdr->e_shoff];
	
	// 获取shstrtab这个节的偏移地址
	char *shstrtab = (char *)&mem[(shdr[ehdr->e_shstrndx].sh_offset)];


	// 获取string tab的偏移地址
	int i = 0;
	i = 0;
	while (strcmp(shstrtab+shdr[i].sh_name, ".strtab") !=0 ) {
		i++;
	}
	char *strtab = (char *)&mem[shdr[i].sh_offset];

	
    // 获取.symtab的索引地址
    i = 0;
	while (strcmp(shstrtab+shdr[i].sh_name, ".symtab") != 0) {
		i++;
	}
	Elf32_Sym *symtab = (Elf32_Sym *)&mem[shdr[i].sh_offset];
	
	// 获取symble tab的项数，以便迭代时使用（将要把fun单独提取出来） 
	int symtab_num = shdr[i].sh_size / sizeof(Elf32_Sym);
	int funtab_num = 0; 
	for (int i = 0; i < symtab_num; i++) {
		if ((symtab[i].st_info & 0x0f) == STT_FUNC) funtab_num ++;
	}
	
	if (funtab_num >= 1000) {
		printf("too many function\n");
		exit(-1);
	};
	
	// 将fun信息提取出来
	symlist.count = funtab_num;
	SymNode *funtab = malloc(sizeof(SymNode)*funtab_num);
	int m = 0;
	for (int i = 0; i < symtab_num; i++) {
		if ((symtab[i].st_info & 0x0f) == STT_FUNC) {
			funtab[m].addr = (symtab[i].st_value);
			strncpy(funtab[m].name, strtab+symtab[i].st_name, 63);
			m++;
		}
	}

  symlist.exist = true;
	symlist.list = funtab;
	
	clos(fd);
  return;
}

char *ftrace(char *pbuf) {
  if (symlist.exit == false) return;
  // 函数调用栈
  typedef struct Fun_Stat {
    int stat[64];
    int p;  // 栈元素总数
  } Fun_Stat;
  static Fun_Stat fun_stat = {.p = -1};

  //00001_000_00000_1100111   0x08067 为 ret指令
  if ((cur_inst & 0xff07f) == 0x8067) {
    uint32_t location = 0;
    int index;
    for (int i = fun_stat.p; i >= 0 ; i--) {
      // 返回位置在dnpc地址之前最近的一个函数
      uint32_t addr = symlist.list[fun_stat.stat[i]].addr;
      if (next_pc > addr && addr > location) {
        location = addr;
        index = i;
      }
    }
    fun_stat.p = index;
    pbuf += sprintf(pbuf, "%2d ret  [#addr:%x] #%s\n", index, cur_pc, symlist.list[fun_stat.stat[index]].name);
  }
  //jal和jalr
  else if ((cur_inst & 0x7f) == 0x6f || (cur_inst & 0x707f) == 0x67){   // 0x6f=11011_11
    for (int i = 0; i < symlist.count; i++) {
      if (next_pc == symlist.list[i].addr) {
        fun_stat.p++;
        if (fun_stat.p > 64) {
          printf("function statck is overflower.\n");
          exit(-1);
        }
        fun_stat.stat[fun_stat.p] = i;  // 将调用过的函数写入栈中以便return时检查
        pbuf += sprintf(pbuf, "%2d call [@addr:%x] @%s\n", fun_stat.p, \
              symlist.list[i].addr, symlist.list[i].name);
      }
    }
  }
  return pbuf;
}

// 放在sim_init中
void trace_init() {
  init_disasm();  // itrace
  ftrace_init();
  file = fopen("trace-log.txt", "w");

  if (file == NULL) {
    printf("Failed to create file");

    exit(1);
  }
  return;
}


// 放在sim_exit()中
void trace_exit() {
  fclose(file);

  // ftrace的收尾
  free(symlist.list);
  symlist.list == NULL;
}

// 放在cpu_exec中
void trace_diff() {
  char *p = logbuf;
  p += sprintf(p, "--------\npc:0x%08x  steps:%lu\n", cur_pc, steps);
  // itrace一直开启，区别在于写不写入文件
  p += sprintf(p, "i:%0x08x  ", cur_inst);
  // 反汇编，将结果写入logbuf
  disassemble(p, sizeof(logbuf), cur_pc, (uint8_t *)&cur_inst, 4);
  iringbuf_write(p);  // 只是将itrace结果写入iringbuf
  if (trace_diff_state.itrace == true) fprintf(file, "%s\n", logbuf);
  if (trace_diff_state.ftrace == true) {
    p += ftrace(p);
    fprintf(file, "%s\n", logbuf);
  }
}
