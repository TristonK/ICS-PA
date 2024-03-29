#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
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

static int cmd_si(char *args) {
	int N;
	if(args==NULL) 
		N=1;
	sscanf(args,"%d",&N);
	cpu_exec(N);
	return 0;
}

static int cmd_info(char *args){
	if (args[0]=='r'){
		for(int i=R_EAX;i<=R_EDI;i++){
			printf("%s: 0x%08x\n",regsl[i],reg_l(i));
	 	 	} 
		printf("eip: 0x%08x\n",cpu.eip);
	 	}
	else if(args[0]=='w'){
		infopoint();
		} 
	return 0; 
}

static int cmd_x(char *args){
	int n;
    vaddr_t addre;
	strtok(args," ");
	char *N =strtok(args," ");
	sscanf(N,"%d",&n);
	args= N+strlen(N)+1;
    sscanf(args,"%x",&addre);
     for(int i=0;i<n;i++) { 
		printf("0x%04x ",vaddr_read(addre,4));
		addre+=4;
		}
	printf("\n");
	return 0;  
}

static int cmd_w(char *args){
	WP *p =new_up(args);
    printf("we have set the watchpoint, the index is %d and the val is %d\n",p->NO,p->val);
	return 0; 
	}
static int cmd_d(char *args){
	int no;
	no=atoi(args);
    delete_wp(no);
	printf("we have delete the watchpoint with no.%d\n",no);
	return 0;
	}
static int cmd_help(char *args);

static int cmd_p(char *args){
	uint32_t ans;
	bool succ;
	succ=1;
//	printf("get");
	ans=expr(args,&succ);
//	printf("111");
	if(succ)
		printf("%d\n",ans);
	else 
		assert(0);
	return 0;
	}
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "excute n steps",cmd_si},
  { "info", "Print the status",cmd_info},
  { "x", "scan the memory",cmd_x},
  { "p", "calculate the expression", cmd_p},
  { "w", "set the watchpoint", cmd_w},
  { "d", "delete the watchpoint with the index", cmd_d}
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

  while (1) {
    char *str = rl_gets();
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
