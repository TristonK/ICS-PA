#include<stdio.h>
#include<nemu.h>
#include<./monitor/expr.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
  FILE *fp=fopen("~/ics2018/nemu/tools/gen-expr/input","r");
  //assert(fp!=NULL);
  uint32_t mach,me;
  char ex[300];
  for(int i=1;i<=100;i++){
      memset(ex,'\0',sizeof(ex));
	  fscanf(fp,"%u",&mach);
	  fscanf(fp,"%[^\n]",ex);
	  bool succ=1;
	  me=expr(ex,&succ);
	  printf("%dth expr: ",i);
	  if(me==mach) printf("answer is true");
	  else printf("answer is WRONG!");	  
	  }
  
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
