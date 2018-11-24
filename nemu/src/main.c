#include<stdio.h>
#include<nemu.h>
#include<monitor/expr.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *e,bool *success);
void test_expr();
int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
 	 int is_batch_mode = init_monitor(argc, argv);
	//test_expr();
   /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
void test_expr(){
	 FILE *fp=fopen("/home/tengwang/ics2018/nemu/tools/gen-expr/input","r");
  		uint32_t ans1;
		uint32_t ans2;
		char expre[400];
		int cnt=1;
	while(fscanf(fp,"%u",&ans1)!=EOF)
	{
		bool success=1;	
		printf("%d:",cnt++);
		fscanf(fp,"%[^\n]",expre);
		ans2=expr(expre,&success);
		if(ans1==ans2)printf("True\n");
		else {
			printf("Flase.input:%u,your:%u,%s\n",ans1,ans2,expre);
		}
	}
	fclose(fp);



}

