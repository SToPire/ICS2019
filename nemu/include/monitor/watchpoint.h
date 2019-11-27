#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char s[211];
  uint32_t ans;
  int num;
  /* TODO: Add more members if necessary */


} WP;

#endif
