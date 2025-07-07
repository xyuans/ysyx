#ifndef __DIFF_DUT_H__
#define __DIFF_DUT_H__

#include <stdint.h>
void init_difftest(char *ref_so_file, long img_size);
bool difftest_step(uint32_t pc, uint32_t npc);
#endif
