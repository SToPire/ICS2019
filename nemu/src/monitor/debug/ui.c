#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display(void);
uint32_t vaddr_read(uint32_t add,int len);
uint32_t expr(char *e, bool *success);
WP* new_wp();
void isa_watch_display();
void free_wp(WP *wp);
void delete(int num);

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

static int cmd_x(char *args){
  if(args==NULL){
    printf("using info N Address to display the value of 4 bytes in hexadecimal form\n");
    return 0;
  }
   char *args_end=args+strlen(args),*now=args;
   while((now<args_end)&&((*now)==' ')) ++now;
   if(now==args_end){
    printf("using info N Address to display the value of 4 bytes in hexadecimal form\n");
    return 0;
   }
   char *token=strtok(now," ");
   int num=atoi(token);
   token=strtok(NULL," ");
   uint32_t add;sscanf(token,"%x",&add);
   token=strtok(NULL," ");
   if(token!=NULL){
     printf("There are too many parameters in your command\n");
     return 0;
   }
   while(num--){
   	printf("0x%08x ",vaddr_read(add,4));
	add+=4;   
   }   
   printf("\n");
   return 0; 
}
static int cmd_info(char *args){
  if(args==NULL){
    printf("using info r to get the value of all registers\n");
    printf("using info w to get the value of all watches\n");
    return 0;
  }
  char *args_end=args+strlen(args),*now=args;
   while((now<args_end)&&((*now)==' ')) ++now;
   if(now==args_end){
    printf("using info r to get the value of all registers\n");
    return 0;
   }
   char *i=now,*last=i;
   while(i<args_end){
     if(((*i)!=' ')) last=i;
     ++i;
   }
   if(last-now>1){
     printf("There exist illegal expressions in your parameters!!!\n");
     return 0;
  }
  if((*now)=='r'){
    isa_reg_display();
    return 0;
  }
  if((*now)=='w'){
  	isa_watch_display();
  	return 0;
  }
  printf("There exist illegal expressions in your parameters!!!\n");
  return 0;
}

static int cmd_si(char *args){
   if(args==NULL){
     cpu_exec(1);
     return 0;
   }
   char *args_end=args+strlen(args),*now=args;
   while((now<args_end)&&((*now)==' ')) ++now;
   if(now==args_end){
    cpu_exec(1);
    return 0;
   }
   char *i=now,*last=i;
   while(i<args_end){
     if(((*i)>='0')&&((*i)<='9')) last=i;
     ++i;
   }
   long long n=0;
   while(now<=last){
     if((*now)<'0'||(*now)>'9'){
       printf("There exit illegal expressions in your parameters!!!\n");
       return 0;
     }
     n=n*10+(*now)-48;
     if(n>1e9) n=1e9;
     ++now;
   }
   cpu_exec(n);
   return 0;
}
static int cmd_p(char *args){
   bool okay=1;
   uint32_t ans=expr(args, &okay);
   if(okay)
		printf("Ans=%u\n",ans);
   else
		printf("There is something wrong in your expression!!!\n");
	return 0;   	
}
static int cmd_w(char *args){
	bool okay=1;
	uint32_t ans=expr(args, &okay);
	if(okay)
		printf("Ans=%u\n",ans);
   else{
		printf("There is something wrong in your expression!!!\n");
		return 0;
	}
	WP *x=new_wp();
	int len=strlen(args);
	for(int i=0;i<=len;++i)
		(*x).s[i]=*(args+i);
	(*x).ans=ans;
	return 0;
}
static int cmd_d(char *args){
	delete(atoi(args));
	return 0;
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
  { "si", "Let the program stop after running N operations",cmd_si},
  { "info", "Display the information what you requires", cmd_info},
  { "x", "Display a continuous range of N 4 bytes in hexadecimal form",cmd_x},
  { "p", "Display the result of an expression",cmd_p},
  { "w", "Set a watch of expressions you give",cmd_w},
  { "d", "Delete a watch of expressions you give",cmd_d}
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
