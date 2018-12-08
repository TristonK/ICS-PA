#include "nemu.h"


/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ ,TK_DINT,TK_NEQ,TK_AND,TK_HEX,TK_DEREF,TK_REG,
  
  /* TODO: Add more token types */

};
extern CPU_state cpu;
static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"-", '-'},            //minus
  {"\\*", '*'},            //multiply
  {",", ','},		//comma逗号
  {"/", '/'},		//除
  {"\\(",'('},		//左圆括号
  {"\\)",')'},		//右圆括号
  {"!=",TK_NEQ},	//不等号
  {"0x[0-9a-f]+",TK_HEX},	//十六进制数
  {"\\$[a-z]+",TK_REG},
  {"&&",TK_AND},	//逻辑与
   {"[0-9]+", TK_DINT},		//十进制整数

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[1000];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
	Assert(sizeof(e)<800,"输入数据太长");
  while (e[position] != '\0') {
	  Assert(nr_token<800,"缓冲区溢出！");
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

    /*    Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
     */ 
      	position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
	case TK_NOTYPE: break;
	 case  '(' 	 : tokens[nr_token++].type='(';break;
 	 case  ')' 	 : tokens[nr_token++].type=')';break;
	 case  '+' 	 : tokens[nr_token++].type='+';break;
	 case  '-' 	 : tokens[nr_token++].type='-';break;
	 case   '*' 	 : tokens[nr_token++].type='*';break;
	 case  '/' 	 : tokens[nr_token++].type='/';break;
         case  TK_DINT  : tokens[nr_token].type=TK_DINT;
		  	 sscanf(substr_start,"%[0-9]",tokens[nr_token].str);
		  	 nr_token++;break;
	 case  TK_EQ 	 : tokens[nr_token++].type=TK_EQ;break;
 	 case  TK_NEQ 	 : tokens[nr_token++].type=TK_NEQ;break;
	 case  TK_AND 	 : tokens[nr_token++].type=TK_AND;break;
	 case  TK_REG    :tokens[nr_token].type=TK_REG;sscanf(substr_start+1,"%[a-z]",tokens[nr_token].str);nr_token++;break;
         case  TK_HEX  : tokens[nr_token].type=TK_HEX;
			   sscanf(substr_start,"%[0-9a-f|0x]",tokens[nr_token].str);
			   nr_token++;break;

	 
 //	'+' 	 : tokens[nr_token].type='+';
 //	'+' 	 : tokens[nr_token].type='+';

          default: TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
int find_match_r(int i,int j);
bool checkparentheses(int i,int j);
int findop(int l,int r);
bool flag;
bool alg_op(int type);
uint32_t get_reg(char *s);
bool check_deref(int type);
	/*找到左端位置为i(i位置为'('),右端位置为j的区间内与i匹配的右括号的位置。if没找到匹配的，就返回0。找到的话返回右括号的位置(肯定大于0)，*/
int find_match_r(int i,int j){
	int t=0;
	int index=i;
	for(index=i;index<=j;index++){
		if(tokens[index].type=='(')t++;
		else if(tokens[index].type==')')t--;
		if(t==0)return index;
	}
	return 0;
}

//判断是否左右括号匹配，匹配的话返回true，否则返回flase.
bool checkparentheses(int i,int j){
	int t=find_match_r(i,j);
	if(t!=j)return false;
	return true;	
}

/*找[l,r]区间范围内的主元的下标。*/
int findop(int l,int r){
	int op=0;
	int pos=0;
	for(int j=l;j<=r;j++){
		if(tokens[j].type=='('){
			j=find_match_r(j,r);
		}
		else if(tokens[j].type==TK_AND){
			op=tokens[j].type;
			pos=j;
		}
		else if(tokens[j].type==TK_EQ||tokens[j].type==TK_NEQ){
			if(op!=TK_AND){
			op=tokens[j].type;
			pos=j;
			}
		}
		else if(tokens[j].type=='+'||tokens[j].type=='-'){
			if(op!=TK_AND&&op!=TK_EQ&&op!=TK_NEQ){
				op=tokens[j].type;
				pos=j;
			}
		}
		else if(tokens[j].type=='/'||tokens[j].type=='*'){
			 if(op=='*'||op=='/'||op==0){op=tokens[j].type;pos=j;}
		}
	}
	return pos;
}
uint32_t get_reg(char *s){

		if(strcmp( "eax",s)==0)return cpu.eax;
		if(strcmp( "ecx",s)==0)return cpu.ecx;
		if(strcmp( "edx",s)==0)return cpu.edx;
		if(strcmp( "ebx",s)==0)return cpu.ebx;
		if(strcmp( "esp",s)==0)return cpu.esp;
		if(strcmp( "ebp",s)==0)return cpu.ebp;
		if(strcmp( "esi",s)==0)return cpu.esi;
		if(strcmp( "edi",s)==0)return cpu.edi;
		if(strcmp("eip",s)==0)return cpu.eip;
		if(strcmp( "ax",s)==0)return cpu.gpr[0]._16;
		if(strcmp( "cx",s)==0)return cpu.gpr[1]._16;
		if(strcmp( "dx",s)==0)return cpu.gpr[2]._16;
		if(strcmp( "bx",s)==0)return cpu.gpr[3]._16;
		if(strcmp( "sp",s)==0)return cpu.gpr[4]._16;
		if(strcmp( "bp",s)==0)return cpu.gpr[5]._16;
		if(strcmp( "si",s)==0)return cpu.gpr[6]._16;
		if(strcmp( "di",s)==0)return cpu.gpr[7]._16;
		if(strcmp( "al",s)==0)return cpu.gpr[0]._8[0];
		if(strcmp( "ah",s)==0)return cpu.gpr[0]._8[1];
		if(strcmp( "cl",s)==0)return cpu.gpr[1]._8[0];
		if(strcmp( "ch",s)==0)return cpu.gpr[1]._8[1];
		if(strcmp( "dl",s)==0)return cpu.gpr[2]._8[0];
		if(strcmp( "dh",s)==0)return cpu.gpr[2]._8[1];
		if(strcmp( "bl",s)==0)return cpu.gpr[3]._8[0];
		if(strcmp( "bh",s)==0)return cpu.gpr[3]._8[1];
 		flag=0;
		printf("no such reg or can't use");
		return 0;
	
	return 0;
}
bool alg_op(int type){
	bool y=0;
	if(type=='+'||type=='-'||type=='*'||type=='/')y=1;
	if(type==TK_EQ||type==TK_NEQ||type==TK_AND)y=1;
	return y;
}
int eval(int l,int r){
	if(flag==0)return 0;
	int ans=0;
	if(l>r){printf("Bad expression!:l>r");flag=0;return 0;}
	else if(l==r){
		if(alg_op(tokens[l].type))
		{	printf("Bad expression:op-left");flag=0;return 0;}
		if(tokens[l].type==TK_REG){return (int)get_reg(tokens[l].str);}
		else if(tokens[l].type==TK_HEX){
		int temp=0;
		sscanf(tokens[l].str,"%x",&temp);
		return temp;
		}
		else if(tokens[l].type==TK_DINT){
		int temp=0;
		sscanf(tokens[l].str,"%d",&temp);
		return temp;
		}
		return 0;
	}
	else if(r-l==1 &&tokens[l].type==TK_DEREF ){
		vaddr_t addr;
		if(tokens[r].type==TK_REG){
			addr=get_reg(tokens[r].str);
		}
		if(tokens[r].type==TK_HEX)sscanf(tokens[r].str,"%x",&addr);
		else sscanf(tokens[r].str,"%u",&addr);
		return (int)vaddr_read(addr,4); 
	}
	else {
		if(checkparentheses(l,r)==true)return eval(l+1,r-1);
		else{
		int index=findop(l,r);
		if(index==0){
			if(tokens[l].type==TK_DEREF)return vaddr_read(eval(l+1,r),4);
			else { printf("Bad expression!referror\n");flag=0;return 0;}
		}
		int op=tokens[index].type;
		int p=eval(l,index-1);
		int q=eval(index+1,r);
		switch(op){
			case '+':ans=p+q;break;
			case '-':ans=p-q;break;
			case '*':ans=p*q;break;
			case '/':ans=p/q;break;
			case TK_AND:ans=(p&&q);break;
			case TK_EQ:ans=(p==q);break;
			case TK_NEQ:ans=(p!=q);break;
			default: flag=0;break;
		}
		}
	}
	return ans;
}
bool check_deref(int type){
	bool deref=0;
	if(type=='+'||type=='-'||type=='*'||type=='/')deref=1;
	if(type==TK_EQ||type==TK_NEQ||type==TK_AND)deref=1;
	if(type=='(')deref=1;
	return deref;
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  for(int i=0;i<nr_token;i++){
  	if(tokens[i].type=='*'&&(i==0 || check_deref(tokens[i-1].type)))tokens[i].type=TK_DEREF;
  }
  flag=1;
  int ans=eval(0,nr_token-1);
  if(flag==1)return ans;
  else *success=false;
  /* TODO: Insert codes to evaluate the expression. */
 // TODO();

  return 0;
}







