#ifndef __FTRACE_H__
#define __FTRACE_H__
typedef struct SymNode {
	uint32_t addr;
	char name[64];
} SymNode;

typedef struct SymList {
	SymNode *first;
	int count;
} SymList;

#endif

