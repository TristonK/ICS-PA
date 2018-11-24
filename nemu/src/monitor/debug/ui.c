#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include<stdarg.h>
void cpu_exec(uint64_t);
WP* new_wp();
void info_w();
void delete_wp(int a);
extern uint32_t  expr(char *e,bool *success);
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
static int cmd_p(char *args){

	bool success=1;
	uint32_t ans=(unsigned)expr(args,&success);
	if(success==false)printf("error");
	else {
		printf("d:%u   x:%x\n",ans,ans);
	}
	return 0;
}
static int cmd_d(char *args){
	int a;
	if(sscanf(args,"%d",&a))delete_wp(a);
	else printf("watchpoint to be deleted should be its number lable\n");
	return 0;	
}
static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}
static int cmd_w(char *args){
	bool success=1;
	uint32_t ans=(unsigned)expr(args,&success);
	if(success==false)printf("expression evaluate fail\n");
	else {
		WP *ptr=new_wp();
		strcpy(ptr->exp,args);
		ptr->oldvalue=ans;
		printf("set a watchpoint: number %d\t %s\n",ptr->NO,args);
	}
	return 0 ;
}
static int cmd_q(char *args) {
  return -1;
}
static int cmd_si(char *args) {
/*	int a=0;	
 	char *b=args;	
	 a=sscanf(args,"%[1-9]",b);
	if(a==0)cpu_exec(1);
	else {
		sscanf(b,"%d",&a);
		cpu_exec(a);
	}
	return 0;
*/
	if(args==NULL){cpu_exec(1);return 0;}
	int b=1;
	sscanf(args,"%d",&b);
	cpu_exec(b);
	return 0;
}
static int cmd_x(char *args){
	char *arg=args;
	char *ptr=NULL;
	if(arg==NULL)printf("Argument required (starting display address).\n");
	else  {
		vaddr_t  addr;
		ptr=strtok(arg," ");
		ptr=strtok(NULL,"\0");
		if(ptr==NULL){
			sscanf(arg,"%x",&addr);
			printf("0x%x:  0x%.8x\n",addr,vaddr_read(addr,4));	
		}
		else {
			int len;
			sscanf(arg,"%d",&len);
			sscanf(ptr,"%x",&addr);
			for(int i=0;i<len;i++){
				printf("0x%x:  %.8x\t",addr,vaddr_read(addr,4));
				addr=addr+4;
				if(i%4==3)putchar('\n');
			}
		}
	}
	return 0;	
}
	CPU_state cpu;

static int cmd_info(char *args){
	if(strcmp(args,"r")==0){
	printf("eax\t0x%.8x   %10d\n",cpu.eax,cpu.eax);
	printf("ebx\t0x%.8x   %10d\n",cpu.ebx,cpu.ebx);
	printf("ecx\t0x%.8x   %10d\n",cpu.ecx,cpu.ecx);
	printf("edx\t0x%.8x   %10d\n",cpu.edx,cpu.edx);
	printf("esi\t0x%.8x   %10d\n",cpu.esi,cpu.esi);
	printf("edi\t0x%.8x   %10d\n",cpu.eax,cpu.edi);
	printf("ebp\t0x%.8x   %10d\n",cpu.ebp,cpu.ebp);
	printf("esp\t0x%.8x   %10d\n",cpu.esp,cpu.esp);
	printf("eip\t0x%.8x   %10d\n",cpu.eip,cpu.eip);
	printf("ebp\t0x%.8x   %10d\n",cpu.ebp,cpu.ebp);
	}
        else if(strcmp(args,"w")==0){
	info_w();
	}
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
  { "si", "Execute just one instruction of the program",cmd_si},
  { "x", "Scan memory",cmd_x},
  { "info", "print program or regs' information",cmd_info},
  {"p","calculate value of the given expression",cmd_p}, 
  {"w","set a watchpoint",cmd_w},
  {"d","delete a watchpoint",cmd_d},
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
