#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  uint32_t val;
  char exp[32];
  /* TODO: Add more members if necessary */
  bool used;
} WP;
WP* new_up(char*);
void free_up(WP*);
void delete_wp(int);
void infopoint();
bool check_point();
#endif
