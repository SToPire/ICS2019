#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

uint32_t isa_reg_str2val(const char *s, bool *success);

enum {
  TK_NOTYPE = 256, DEREF, TK_EQ, TK_NEQ, TK_AND, 
  TK_NUM, TK_ZERO, TK_HEX_NUM, TK_HEX_ZERO, TK_REG, 

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"u", TK_NOTYPE},		// u(unsigned)
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-" , '-'},			// sub
  {"\\*", '*'},  		// multiply
  {"/", '/'},			// divide
  {"\\(", '(' },		// left bracket
  {"\\)", ')' },		// right bracket
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},		// nonequal
  {"&&", TK_AND},		// and
  {"0x0*[1-9a-fA-F][0-9a-fA-F]*",TK_HEX_NUM},//hex_num
  {"0x0+",TK_HEX_ZERO}, 		//hex_zero	
  {"0*[1-9][0-9]*",TK_NUM},		// num
  {"0+",TK_ZERO},				//zero
  {"\\$[a-zA-Z]+",TK_REG},		//reg	
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

static Token tokens[1024] __attribute__((used)) = {};
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

       /*Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);*/
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
			case TK_NOTYPE: break;
			case '+': 	tokens[nr_token].type='+';
					  	strcpy(tokens[nr_token].str,"+");
					  	++nr_token;
					  	break;
			case '-': 	tokens[nr_token].type='-';
						strcpy(tokens[nr_token].str,"-");
						++nr_token;
						break;
			case '*': 	tokens[nr_token].type='*';
						strcpy(tokens[nr_token].str,"*");
						++nr_token;
						break;
			case '/': 	tokens[nr_token].type='/';
						strcpy(tokens[nr_token].str,"/");
						++nr_token;
						break;
			case '(': 	tokens[nr_token].type='(';
						strcpy(tokens[nr_token].str,"(");
						++nr_token;
						break;
			case ')': 	tokens[nr_token].type=')';
						strcpy(tokens[nr_token].str,")");
						++nr_token;
						break;
			case TK_EQ: tokens[nr_token].type=TK_EQ;
					  	strcpy(tokens[nr_token].str,"==");
					  	++nr_token;
					  	break;
			case TK_NEQ:tokens[nr_token].type=TK_NEQ;
					  	strcpy(tokens[nr_token].str,"!=");
					  	++nr_token;
					  	break;
			case TK_AND:tokens[nr_token].type=TK_AND;
					  	strcpy(tokens[nr_token].str,"&&");
					  	++nr_token;
					  	break;
			case TK_REG:		tokens[nr_token].type=TK_REG;
								++substr_start;--substr_len;
								sprintf(tokens[nr_token].str,"%.*s",substr_len,substr_start);
								++nr_token;
						 		break;							
			case TK_NUM:		tokens[nr_token].type=TK_NUM;
							 	while(*substr_start=='0' && substr_len>1){ ++substr_start; --substr_len; } //delete 0 in prefix
						 		sprintf(tokens[nr_token].str,"%.*s",substr_len,substr_start);
						 		++nr_token;
						 		break;
			case TK_ZERO:
			case TK_HEX_ZERO:	tokens[nr_token].type=TK_NUM;
							 	strcpy(tokens[nr_token].str,"0");
						 		++nr_token;
						 		break;
			case TK_HEX_NUM: 	tokens[nr_token].type=TK_NUM;
								substr_start+=2;substr_len-=2;
						 		while(*substr_start=='0' && substr_len>1){ ++substr_start; --substr_len; } //delete 0 in prefix
								unsigned tmp;
						 		sscanf(substr_start,"%x",&tmp);
							 	sprintf(tokens[nr_token].str,"%d",tmp);
							 	++nr_token;
							 	break;
								
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

int check_parentheses(int p, int q){
		/*
		 *	return 0: illegal expression
		 *	return 1: legal expression, and brackets match in begin and end
		 *	return -1:legal expression, but brackets don't match in begin ane end
		 */
	while(tokens[p].type=='-') ++p; // delete '-' in prefix
	if(p==q && tokens[p].type==TK_NUM) return -1;// a single number is legal
	int i;
	int cnt=0;
	bool flag=false;
	for(i=p;i<=q;i++){
		if(tokens[i].type=='(') ++cnt;
		else if(tokens[i].type==')') --cnt;
		if(cnt<0) return 0;
		if((cnt==0 && (i!=p && i!=q))) flag=true;
	}
	if(cnt) return 0;
	else if(flag) return -1;
	else if(tokens[p].type=='(' && tokens[q].type==')') return 1;
	else return 0;
}

int find_main_operator(int p,int q){
		/*
		 *	return -1:	no main operator found in the expression
		 *	return i: 	main operator found in tokens[i]
		 */
	int in_brackets=0; 	// wrapped in how many levels of brackets
	int now=4; 			// priority of matched operator now. 4:'*','/'; 3:'+','-'; 2:'==','!='; 1:'&&'
	int ans=-1;
	int i;
	for(i=p;i<=q;i++){
		if(tokens[i].type=='(') ++in_brackets;
		else if(tokens[i].type==')') --in_brackets;
		else if((tokens[i].type=='*' || tokens[i].type=='/')
				&& now>=4 && !in_brackets)  
					{ans=i;now=4;}
		else if((tokens[i].type=='+')
				&& now>=3 && !in_brackets)
					{ans=i;now=3;}
		else if(tokens[i].type=='-' && now>=3 && !in_brackets && i!=p 
				&& tokens[i-1].type!='+'
				&& tokens[i-1].type!='-'
				&& tokens[i-1].type!='*'
				&& tokens[i-1].type!='/')
					{ans=i;now=3;}
		else if(tokens[i].type==TK_EQ 
				&& now>=2 && !in_brackets)
					{ans=i;now=2;}
		else if(tokens[i].type==TK_NEQ 
				&& now>=2 && !in_brackets)
					{ans=i;now=2;}
		else if(tokens[i].type==TK_AND
				&& now>=1 && !in_brackets)
					{ans=i;now=1;}
	}
	return ans;
}

uint32_t eval(int p,int q){
	if(p>q){
		printf("Bad Expression!\n");
		return 0;
	}
	else if(p==q){
		if(tokens[p].type==TK_NUM) return (uint32_t)atoi(tokens[p].str);
		else{
			printf("Bad Expression!\n");
			return 0;
		}
	}
	else if(tokens[p].type=='-' && ((q-p==1 || ( check_parentheses(p+1,q)!=0 && find_main_operator(p+1,q)==-1)))) return -eval(p+1,q);
	else if(tokens[p].type==DEREF && ((q-p==1 || ( check_parentheses(p+1,q)!=0 && find_main_operator(p+1,q)==-1)))) return vaddr_read(eval(p+1,q),4);
	else{
		int v=check_parentheses(p,q);
		if(v==1) return eval(p+1,q-1); 			// match brackets
		else if(v==0){
			printf("Bad Expression!\n");
			return 0;
		}										// illegal expression
		else if(v==-1){
			int op=find_main_operator(p,q);		// legal, but p & q don't match
			uint32_t val1=eval(p,op-1);
			uint32_t val2=eval(op+1,q);
			switch(tokens[op].type){
				case '+':return (uint32_t)((uint32_t)val1 + val2);
				case '-':return (uint32_t)((uint32_t)val1 - val2);
				case '*':return (uint32_t)((uint32_t)val1 * val2);
				case '/':return (uint32_t)((uint32_t)val1 / val2);
				case TK_EQ:return val1 == val2;
				case TK_NEQ:return val1 != val2;
				case TK_AND:return val1 && val2;
			}
		}
	}
	return 23333; //will not be executed forever, just for pass the compliation.
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  int i;
  for(i=0;i<nr_token;i++){
  	if(tokens[i].type==TK_REG){			//寄存器，用值替换
  		uint32_t t=isa_reg_str2val(tokens[i].str,success);
  		if(*success){tokens[i].type=TK_NUM;sprintf(tokens[i].str,"%u",t);}
  		else return 0;
  	}
  	else if(tokens[i].type=='*'){
  		int j=i;
  		do{ --j;} while(tokens[j].type=='(' || tokens[j].type==')');
  		if(!(tokens[j].type==TK_NUM) || i==0) tokens[i].type=DEREF;
  	}
  }
  return eval(0,nr_token-1);
}
