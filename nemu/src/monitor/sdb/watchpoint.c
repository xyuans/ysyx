/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32
#define EXP_MAX 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expression[EXP_MAX];
  uint32_t prev_value;
  uint32_t current_value;
} WP;

// 改为使用更丰富的链表
typedef struct UsedPointList {
  int count;
  WP *head;
}UPL;

static WP wp_pool[NR_WP] = {};
static WP *free_ = NULL;    // 删除原来使用链表头节点
static UPL usd_wpl = {};

void init_wp_pool() {
  int i;
  // 创建链表结构
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }
  
  // 初始化free_
  free_ = &wp_pool[0];
}

/* TODO: Implement the functionality of watchpoint */

// 在头部插入节点,释放free_头节点
void new_wp(char *e) {
  assert(usd_wpl.count < NR_WP);

  bool success = true;
  uint32_t value = expr(e, &success);
  if (success == false) {
    printf("can not set watchpoint.\n");
    return;
  }

  usd_wpl.count = usd_wpl.count + 1;
  WP *tmp = free_->next;
  free_->next = usd_wpl.head;  // 在头部插入，让最新的监视点在原头节点之前
  usd_wpl.head = free_;  // 让最新的监视点成为头节点
  free_ = tmp;  // free_指向原空闲链表头节点的下一个
  
  usd_wpl.head->prev_value = value;
  strncpy(usd_wpl.head->expression, e, EXP_MAX-1);
}

// 释放指定节点，在free_头插入
void free_wp(int NO) {
  if (usd_wpl.count == 0) {
    return;
  }
  int i;
  WP *current_wp = usd_wpl.head;
  
  /*只有一个节点的情况*/
  if (usd_wpl.head->NO == NO) {
     wp_pool[NO].next = free_;  // 释放的节点指向原来的头节点
     free_ = &wp_pool[NO];      // 改变free_头节点指向
     usd_wpl.count--;
     usd_wpl.head = usd_wpl.head->next;
     return;
  }
  /**/
  for (i = 0; i < usd_wpl.count; i++) {
    if (current_wp->next->NO == NO) {
      /*改变原链表结构*/
      current_wp->next = current_wp->next->next;
      usd_wpl.count--;

      wp_pool[NO].next = free_; // 释放的节点指向原来的空闲链表的头节点
      free_ = &wp_pool[NO];     // 改变空闲链表头指针指向
    
      return;
    }
    current_wp = current_wp->next;
  }
}

// 比较监视点当前值与之前的值
int check_wps() {
  bool success = true;
  
  int i;
  int count = usd_wpl.count;
  WP *current_wp = usd_wpl.head;

  for(i = 0; i < count; i++) {
    current_wp->current_value = expr(current_wp->expression, &success);
    if (current_wp->prev_value != current_wp->current_value) {
      printf("Watchpoint: %d\n\nOld value: %d\nNew value: %d\n",
              current_wp->NO, current_wp->prev_value, current_wp->current_value);

      current_wp->prev_value = current_wp->current_value;

      return 1;
    }
    current_wp = current_wp->next;
  }
  return 0;
}


// 打印监视点
void print_wps() {
  int count = usd_wpl.count;
  WP *current_wp = usd_wpl.head;
  if (count == 0) {
    printf("no watchpoint\n");
    return;
  }
  printf("Num     What\n");
  for (int i= 0; i < count; i++) {
    printf("%-8d%s\n", current_wp->NO, current_wp->expression);
    current_wp = current_wp->next;
  }
  return;
}
