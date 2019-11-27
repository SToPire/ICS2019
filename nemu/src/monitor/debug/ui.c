#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display(void);
uint32_t expr(char *e, bool *success);
void free1(int now);
void show(void);
void issert(char *args,uint32_t ans);
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_p(char *args) {
  bool success=true;
  uint32_t  ans;
  ans=expr(args,&success);
  if(success) 
  {
    printf("%u\n",ans);
  }
  else printf("gg\n");
  return 0;
}
static int cmd_q(char *args) {
  return -1;
}
static int cmd_d(char *args) {
  int now=atoi(args);
  free1(now);
  return 0;
}
static int cmd_w(char *args) { 
  bool success=true;
  uint32_t ans;
  ans=expr(args,&success);
  issert(args,ans);
  return 0;
}
static int cmd_x(char *args) {
  char *arg =strtok(NULL," ");
  if(!(arg==NULL))
  { 
	int i=atoi(arg);
	char *args=strtok(NULL," ");
	int k;
        uint32_t j;
	bool success=true;
        j=expr(args,&success);
	for(k=0;k<i;k++)
	{
          printf("0x%x    0x%02X%02X%02X%02X\n",j+k*4,pmem[j+k*4],pmem[j+k*4+1],pmem[j+k*4+2],pmem[j+k*4+3]);	  
	}
	return 0;
  }
  else return 0;
}
static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  int i;
  if (arg == NULL) {  
     cpu_exec(1);
     return 0;
  }
  else {
     i=atoi(arg);
     cpu_exec(i);
     return 0;
  }
}
static int cmd_info(char *args) { 
  char *arg = strtok(NULL," ");
  if(arg == NULL) {
    return 0;
  }
  else { 
    if(strcmp(arg,"r")==0)
    {
      isa_reg_display();
      return 0;
    }
    else 
    if(strcmp(arg,"w")==0)
    {
        show();	
	return 0;
    }	
    else return 0;
  }
}	   

static int cmd_help(char *args);
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "get into  the next step or next step", cmd_si },
  { "info", "check the situation of the register and watchpoint", cmd_info },
  { "p", "calculate the result of the expr", cmd_p },
  { "x", "scan the main memory", cmd_x},
  { "w", "set a watchpoint",cmd_w},
  { "d", "delete a watchpoint",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;
  
  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
