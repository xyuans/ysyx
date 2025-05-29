#ifndef __FTRACE_H__
#define __FTRACE_H__
#include <stdbool.h>
typedef struct SymNode {
  uint32_t addr;
	char name[64];
} SymNode;

typedef struct SymList {
	bool exist;
  SymNode *list;
	int count;
} SymList;

#endif

