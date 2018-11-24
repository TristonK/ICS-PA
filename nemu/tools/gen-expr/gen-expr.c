#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
int size=0;
int rannum=0;
int cnt=0;
char gen_rand_op();
static inline void gen_rand_expr() {
  srand(time(0)+rannum);
  rannum++;
  int t=rand()%3;
  if(t==1)buf[size++]=' ';
  if(size>=60000)return ;
  int d;
  if (cnt>=100)t=0;
  
  switch(t){
  	case 0: d=rand()%0x200;
		size+=sprintf(&buf[size],"%d",d);
		cnt++;
	      break;
	case 1:buf[size++]='(';cnt++;gen_rand_expr();buf[size++]=')';break;
	case 2 :cnt++;gen_rand_expr();buf[size++]=gen_rand_op();cnt++;gen_rand_expr();break;
  	default:break;
  }
  if(t==2)buf[size++]=' ';

}

char gen_rand_op()
{
	srand(time(0)+rannum);
	rannum++;
	int t=rand()%4;
	switch(t){
  	case 0:return '+';
	case 1:return '-';
	case 2:return '*';
	default :return '/';
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned  result = %s; "
"  printf(\"%%u\\n\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	  size=0;cnt=0;
	  memset(buf,0,sizeof(buf));
	memset(code_buf,0,sizeof(code_buf));
      	  gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
    if (ret != 0) continue;

    fp = popen("./.expr", "r");

    assert(fp != NULL);
	 char check[32];
		 memset(check,'\0',sizeof(check));
    fread(check,1,12,fp);
	if(check[0]=='\0'){continue;}
    int result;
    sscanf(check,"%d",&result);
   // fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
