#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display();
WP* new_wp();
void free_wp(int n);
void WP_disp();

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

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args){
  char *arg = strtok(NULL, " ");
  if (arg == NULL){
		/* no arguments given, with default value 1 */
		cpu_exec(1);	
  }
  else{
		int num = atoi(arg);
		if(!num) printf("Invalid argument! Only positive integer allowed.\n");
		else cpu_exec(num);
  }
  return 0;
}

static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if (arg == NULL){
  		/* no arguments given, print a message */
		printf("Usage: info [r][w]\n");
  }
  else if(strcmp(arg,"r")==0){
		isa_reg_display();
  }
  else if(strcmp(arg,"w")==0){
		WP_disp();
  }
  else{
		printf("Usage: info [r][w]\n");
  }
  return 0;
}

static int cmd_x(char *args){
	/*partially finished, only 0x number acceptable */
  char *arg1 = strtok(NULL," ");
  char *arg2 = strtok(NULL," ");
  if(arg1==NULL){
		printf("Usage: x [n][address]\n");
  }
  else if (arg2==NULL){
		printf("Usage: x [n][address]\n");
  }
  else{
		uint32_t times=atoi(arg1);
		uint32_t addr_head=0;
		sscanf(arg2,"0x%x",&addr_head);
		if(!times || !addr_head) printf("Usage: x [n][address]\n");
		else{
			uint32_t i;
			for(i=0;i<times;i++){
				printf("0x%x: 0x%08x\n",addr_head+4*i,vaddr_read(addr_head+4*i,4));
			}
		}
  }
  return 0;
}

static int cmd_p(char *args)
{
  if(args==NULL){
	  printf("Usage: p [expr]\n");
  }
  else{
	  bool success=true;
	  uint32_t tmp=expr(args,&success);
	  if(success) printf("%u\n",tmp);
	  else printf("Error in evaluation.\n");
  }
  return 0;
}

static int cmd_w(char *args)
{
  if(args==NULL){
	  printf("Usage: w [expr]\n");	
  }
  else{
   	  bool success=true;
  	  uint32_t tmp=expr(args,&success);
  	  if(success){	
		  WP* newWP=new_wp();
		  newWP->val=tmp;
		  strcpy(newWP->what,args);
		  printf("Watchpoint:%d What:%s Value:%u\n",newWP->NO,newWP->what,newWP->val);
	  } 
	  else printf("Error in evaluation.\n");
  }	
  return 0;
}
static int cmd_d(char *args)
{
  if(args==NULL){
	  printf("Usage: d [expr]\n");
  }
  else{
  	  int i=atoi(args);
  	  free_wp(i);
  }
  return 0;
}
static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help 	 },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Usage: si [n]\nStep N instructions.",cmd_si },
  { "info","Usage: info [r][w]\nGeneric command for showing things about the program being debugged.", cmd_info  },
  { "x","Usage: x [n][address]\nExamine memory. ",cmd_x },
  { "p","Usage: p [expr]\nprint the value of a expression.",cmd_p},
  { "w","Usage: w [expr]\nset a watchpoint.",cmd_w},
  { "d","Usage: d [expr]\ndelete a watchpoint.",cmd_d},
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
