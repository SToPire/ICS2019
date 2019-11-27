#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[65535];
  int used;
  uint32_t now;
  /* TUDO: Add more members if necessary */


} WP;

#endif
