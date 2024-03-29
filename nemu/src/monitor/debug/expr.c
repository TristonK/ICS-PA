#include "nemu.h"
#include<stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,DEC,HEX,REG,AND,NOT_EQ,DEREF

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */ 

  {" +", TK_NOTYPE},    // spaces
  {"0[xX][0-9a-fA-F]+", HEX}, //hex number
  {"\\$[a-zA-Z]+",REG},   //register
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"\\-", '-'},         //minus
  {"\\*", '*'},         //multi
  {"\\/", '/'},         //div
  {"[0-9]+", DEC},       //decimal number
  {"\\(", '('},          
  {"\\)", ')'},
  {"&&", AND},
  {"!=", NOT_EQ}
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

  for (i  = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;  
  char str[500];
} Token;
Token tokens[65532];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
	for (i = 0; i < NR_REGEX; i ++) {
       if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

      //  Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

         /* TODO:  Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
         switch (rules[i].token_type) {
			case TK_NOTYPE: break;
          default:
		        assert(substr_len<=32); 
		        tokens[nr_token].type=rules[i].token_type;
		  //  sscanf(substr_start,"%[0-9]",tokens[nr_token].str);
			strncpy(tokens[nr_token].str,substr_start,substr_len);
				tokens[nr_token].str[substr_len]='\0';
				nr_token++;
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
bool check_parentheses(int p,int q){
	if(tokens[p].type=='('&&tokens[q].type==')'){
		int l=0,r=0;
		for(int i=p+1;i<q;i++){
			if(tokens[i].type=='(') l++;
			if(tokens[i].type==')') r++;
			if(r>l){
				//assert(0);
				return false;
		 		} 
		 	}
		if(r==l) return true;
		else return false;
		} 
	else 
	   return false;
	}
int getprecedence(int a){
	//if(tokens[a].type==AND) return 1;
	if(tokens[a].type=='+'||tokens[a].type=='-') return 2;
	if(tokens[a].type=='*'||tokens[a].type=='/') return 3;
	else return 4;
	}
int find_main_op(int p,int q){
	int op=p;
	int opr=p;
	while (op<q){ 
		if(tokens[op].type==DEC||tokens[op].type==HEX||tokens[op].type==REG) {
			op++;continue;}
 		else if(tokens[op].type=='('){
			int blacnt=1;op++;
			while(blacnt!=0){
				if(tokens[op].type==')') blacnt--;
				if(tokens[op].type=='(') blacnt++;
				op++;
				}
			//while(tokens[op].type!=')') op++;
			}
         else if(getprecedence(op)<=getprecedence(opr)){
			opr=op;
			op++;
			} 
		else op++;
 		}
	//printf("%c",tokens[opr].type);
	return opr;
	}
int  eval(int p,int q){
 //	printf("do the func");
  //  printf("%d %d\n",p,q);
	if(p>q){ 
	//	printf("%d %d %d",p,q,tokens[p].type);
        printf("tokens path error");
		return -1;} 
 	else if (p==q){
		if(tokens[p].type==DEC){
 			return atoi(tokens[p].str);}
 		else if(tokens[p].type==HEX){
			char *stt;
			return strtol(tokens[p].str,&stt,16);
			}
		else if(tokens[p].type==REG){
			if(!strcmp(tokens[p].str,"$eax")) return cpu.eax;
			else if(!strcmp(tokens[p].str,"$ecx")) return cpu.ecx;
            else if(!strcmp(tokens[p].str,"$edx")) return cpu.edx;
			else if(!strcmp(tokens[p].str,"$edx")) return cpu.edx;
			else if(!strcmp(tokens[p].str,"$ebx")) return cpu.ebx;
			else if(!strcmp(tokens[p].str,"$esp")) return cpu.esp;
			else if(!strcmp(tokens[p].str,"$ebp")) return cpu.ebp;
			else if(!strcmp(tokens[p].str,"$esi")) return cpu.esi;
		    else if(!strcmp(tokens[p].str,"$edi")) return cpu.edi;
			else if(!strcmp(tokens[p].str,"$ax")) return cpu.gpr[0]._16;
			else if(!strcmp(tokens[p].str,"$cx")) return cpu.gpr[1]._16;
			else if(!strcmp(tokens[p].str,"$dx")) return cpu.gpr[2]._16; 
			else if(!strcmp(tokens[p].str,"$bx")) return cpu.gpr[3]._16;
	        else if(!strcmp(tokens[p].str,"$sp")) return cpu.gpr[4]._16;
	 	    else if(!strcmp(tokens[p].str,"$bp")) return cpu.gpr[5]._16;
		    else if(!strcmp(tokens[p].str,"$si")) return cpu.gpr[6]._16;
		    else if(!strcmp(tokens[p].str,"$di")) return cpu.gpr[7]._16;
			else if(!strcmp(tokens[p].str,"$al")) return cpu.gpr[0]._8[0];
			else if(!strcmp(tokens[p].str,"$cl")) return cpu.gpr[1]._8[0];
			else if(!strcmp(tokens[p].str,"$dl")) return cpu.gpr[2]._8[0]; 
			else if(!strcmp(tokens[p].str,"$bl")) return cpu.gpr[3]._8[0]; 
			else if(!strcmp(tokens[p].str,"$ah")) return cpu.gpr[0]._8[1]; 
			else if(!strcmp(tokens[p].str,"$ch")) return cpu.gpr[1]._8[1]; 
			else if(!strcmp(tokens[p].str,"$dh")) return cpu.gpr[2]._8[1]; 
			else if(!strcmp(tokens[p].str,"$bh")) return cpu.gpr[3]._8[1];
			else if(!strcmp(tokens[p].str,"$eip")) return cpu.eip;
			else {
				printf("no such register\n");
				return -1;
				}  }
		else {
            printf("error:no number input");
			return -1;
		} 
	}
    else if (check_parentheses(p,q)==true){
        //printf("%d",)
		return eval(p+1,q-1);
 		} 
	else{
		//printf("have enter the else part");
		int op=find_main_op(p,q);
		//printf("%d",op);
		if(tokens[op].type==DEREF){
			uint32_t val=eval(op+1,q);
			return vaddr_read(val,4);
			}
		int  val1=eval(p,op-1);
	//	printf("ppp: %d %d %u \n", p,op-1,val1);
		int  val2=eval(op+1,q);
	//	printf("pp2%d %d %u\n ",op+1,q,val2);
		switch(tokens[op].type){
			case '+' : return val1+val2; break;
			case '-' : return val1-val2; break;
			case '*' : return val1*val2; break;
			case '/' : return val1/val2; break;
			case TK_EQ:   return val1==val2; break;
			case NOT_EQ: return val1!=val2; break;
			case AND: return val1&&val2; break;
			default:
			       printf("wrong operation\n"); 
			       assert(0);
  		 	}
  		} 
		return 0;
}
uint32_t expr(char *e, bool *success) {
 // meet(tokens,0,sizeof(tokens));
  if (!make_token(e)) {
    *success = false;
	printf("wrong");
    return 0;
  } 
  //printf("pointer is ok");
  /* TODO: Insert codes to evaluate the expression. */
  for(int i=0;i<nr_token;i++){
	  if(tokens[i].type=='*'&&(i==0||(tokens[i-1].type!=DEC&&tokens[i-1].type!=HEX&&tokens[i-1].type!=')')))
	    tokens[i].type=DEREF;
	  }
  return eval(0,nr_token-1);

  //return 0;
}
