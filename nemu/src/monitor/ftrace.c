#include <generated/autoconf.h>

#ifdef CONFIG_TRACE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <elf.h>
#include <fcntl.h>  // 包含标志位定义 （O_RDONLY）
#include <sys/stat.h>  // 使用O_CREAT时需包含
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>
#include <monitor/ftrace.h>

SymList symlist; //__attribute__((section(".bss")));

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
	char *shstrtab = (char *)&mem[(shdr[ehdr->e_shstrndx].sh_offset)];  // 获取shstrtab这个节的偏移地址
	
	// 获取.symtab在Section header的索引
	int i = 0;
	while (strcmp(shstrtab+shdr[i].sh_name, ".symtab") != 0) {
		i++;
	}
	// 获取.symtab的索引地址
	Elf32_Sym *symtab = (Elf32_Sym *)&mem[shdr[i].sh_offset];
	
	// 获取string tab的偏移地址
	i = 0;
	while (strcmp(shstrtab+shdr[i].sh_name, ".strtab") !=0 ) {
		i++;
	}
	char *strtab = (char *)&mem[shdr[i].sh_offset];
	
	// 获取symble tab中fun的项数，以便迭代时使用（将要把fun单独提取出来） 
	int symtab_num = shdr[i].sh_size / sizeof(Elf32_Sym);
	int funtab_num = 0; 
	for (int i = 0; i < symtab_num; i++) {
		if (symtab[i].st_info == STT_FUNC) funtab_num ++;
	}
	
	if (funtab_num >= 1000) {
		printf("too many function\n");
		exit(-1);
	};
	
	
	// 将fun信息提取出来
	symlist.count = funtab_num;
	SymNode *funtab = malloc(sizeof(SymNode)*funtab_num);
	funtab_num = 0;
	for (int i = 0; i < symtab_num; i++) {
		if (symtab[i].st_info == STT_FUNC) {
			funtab->addr = (symtab[i].st_value);
			strncpy(funtab->name, strtab+symtab[i].st_name, 63);
		}
	}
	symlist.first = funtab;
		
	
	return;
}

#endif



