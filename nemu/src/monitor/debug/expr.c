#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

uint32_t isa_reg_str2val(const char *s,bool *success);
enum {
  TK_NOTYPE = 256, TK_EQ = 257,TK_NEQ = 258 , TK_AND = 259 ,
  TK_NUM = 260 , TK_REG = 261,
  DEREF = 262,TK_MNS = 263,TK_HEXANUM = 264, TK_PC =265
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence leveIl of different rules.
   */
  {"0x[(0-9)|(A-F)]+",TK_HEXANUM}, //HEXANUM
  {"[0-9]+",TK_NUM},     //NUM
  {" +", TK_NOTYPE},    // spaces
  {"\\(", '('},          // (
  {"\\)", ')'},          // )
  {"\\*", '*'},         // mul
  {"/", '/'},           // div
  {"\\+", '+'},        //plus
  {"-", '-'},          //minus
  {"==", TK_EQ},    // equal
  {"!=", TK_NEQ},    // not equal 
  {"&&",TK_AND},     // and
  {"\\$pc",TK_PC}, 
  {"\\$eax|\\$ebx|\\$ecx|\\$edx|\\$esi|\\$edi|\\$esp|\\$ebp",TK_REG}  //REGISTER
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

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

static Token tokens[512] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

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

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
	if(rules[i].token_type!=TK_NOTYPE)
	{
	nr_token++;
        tokens[nr_token].type=rules[i].token_type;
	for(int j=0;j<substr_len;j++)
	tokens[nr_token].str[j]=*(substr_start+j);
	tokens[nr_token].str[substr_len]='\0';
	}
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

      //  switch (rules[i].token_type) {
          
	//  default: TODO();
      //  }

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
static bool check_parentheses(int p,int q){
	int kbb=1;
	if((tokens[p].type=='(') && (tokens[q].type==')'))
	{
	  int num=0;
	  for(int i=p+1;i<=q-1;i++)
	  {
	    if(tokens[i].type=='(') num++;
	    if (tokens[i].type==')') num--;
            if(num<0) kbb=0;
	  }
        }
	else kbb=0;
	if(kbb==1) return true;
	else
	return false;
}
static uint32_t  eval(int p,int q,bool *success) {  
  int i;
  int jl=1;
  int kk=p;
  if(p>q) {  
    printf("?");
    *success=false;
    return 0;
  }
  else if(p==q) {
	  if(tokens[p].type==TK_NUM) {
		  return(atoi(tokens[p].str));
	  }
	  else
          if(tokens[p].type==TK_REG)
	  {
	    return(isa_reg_str2val(tokens[p].str,success));
	  }
	  else
          if(tokens[p].type==TK_HEXANUM)
	  {
            uint32_t kb;
	    sscanf(tokens[p].str,"%x",&kb);
	    return(kb);
	  }
	  else
          if(tokens[p].type==TK_PC)
	  {
	    return(cpu.pc);
	  }
	  else
	  {
            *success=false;
	    return 0;
	  }
  }
  else if(check_parentheses(p,q)==true){
	  return eval(p+1,q-1,success);
  }
  else{
	  int op=-1;
	  int parents=0;
	  for(i=p;i<=q;i++) {
		  if(tokens[i].type=='(') parents++;
		  else
		  if(tokens[i].type==')') parents--;
		  else
		  if(parents==0)
		  {
		    if((tokens[i].type=='*') || (tokens[i].type=='/')){
	                 if(op==-1) op=i;
		         else
                         if((tokens[op].type=='*') || (tokens[op].type=='/')){
			    op=i;
			 }
	            }
		    else
                    if((tokens[i].type=='+') || (tokens[i].type=='-')){
			 if(op==-1) op=i;
			 else
			 if((tokens[op].type!=TK_EQ) && (tokens[op].type!=TK_NEQ) && (tokens[op].type!=TK_AND))   op=i;
	            }
		    else 
	            if((tokens[i].type==TK_EQ) || (tokens[i].type==TK_NEQ)
                    || (tokens[i].type==TK_AND)) op=i;
		    else op=op;
		  }
	   }
	   if(op==-1)
	   {
	     if(tokens[kk].type==TK_MNS)
	     {
	       while(tokens[kk].type==TK_MNS&&kk<=q)
               {
	         kk++;
	         jl=jl*(-1);
               }
               return(jl*eval(kk,q,success));
	     }
	     else
             if(tokens[kk].type==DEREF)
	     {
		     return(pmem[eval(kk+1,q,success)]);
	     }
	     else 
	     {
		*success=false;
		return 0;
	     }
	   }
           else
	   {
	     uint32_t val1=eval(p,op-1,success);
	     uint32_t val2=eval(op+1,q,success);
             switch(tokens[op].type){
	       case '+': return val1+val2;
	       case '-': return val1-val2;
	       case '*': return val1*val2;
	       case '/':
	       if (val2==0)
	       {
		 *success=false;
		 //printf("gg\n");
	         return 0;
	       }  else return val1/val2;
	       case TK_EQ: return(val1==val2);
	       case TK_NEQ: return(val1!=val2);
	       case TK_AND: return(val1&&val2);
	      default: assert(0);
	     }
	  }  
  }
}
		  
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for(int i=1;i<=nr_token;i++){
	  if(tokens[i].type=='*')
	  {
             if(i==1) tokens[i].type=DEREF;
	     if(i>1)
	     {
	       if ((tokens[i-1].type=='+') ||  (tokens[i-1].type=='-') 
		    || (tokens[i-1].type=='*') || (tokens[i-1].type=='/'))
	       {
		       tokens[i].type=DEREF;
	       }
	     }
	  }
	  if(tokens[i].type=='-')
	  {
	    if((tokens[i-1].type==')')||(tokens[i-1].type==TK_NUM)||(tokens[i-1].type==TK_HEXANUM)||(tokens[i-1].type==TK_REG))
	    {
		  tokens[i].type='-';
	    }
	    else tokens[i].type=TK_MNS;
	  }
	  if(tokens[i].type==TK_REG)
	  {
	    strncpy(tokens[i].str,tokens[i].str+1,3); 
	    tokens[i].str[3]='\0';
	  }
  }
  uint32_t  ans;
  ans=eval(1,nr_token,success);
  return ans;
  /* TODO: Insert codes to evaluate the expression. */
  TODO();
}
