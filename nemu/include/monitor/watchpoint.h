#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  struct watchpoint *p;
  /* TODO: Add more members if necessary */
  char exp[100];
	uint32_t oldvalue;
	int hit_time;	
} WP;

#endif
