#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

uint32_t isa_reg_str2val(const char *s, bool *success);
enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM,TK_REG,TK_UEQ,TK_AND,TK_HNUM
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
  {"0x[0-9A-Fa-f]+",TK_HNUM},   //num
  {"[0-9]+",TK_NUM},   //num
  
  {"\\(", '('},         //left 
  {"\\*", '*'},         //multiply
  {"\\)", ')'},         //right 
  {"\\/", '/'},         //div
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\$[a-zA-Z]*",TK_REG}, //resgi
  {"==", TK_EQ},        // equal
  {"!=", TK_UEQ},        // unequal
  {"&&",TK_AND}
  
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
  	//printf("%d\n",i);
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[64];
} Token;

static Token tokens[2000] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  //printf("yes\n");
  init_regex();
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      //int len=strlen(e);
      //printf("%s\n",e);
      //e[len]='\0';
      //regexec(&re[i], e + position, 1, &pmatch, 0) ;
      //printf("%d\n",pmatch.rm_so);printf("yes\n");
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
       //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
           // i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
            case '+':tokens[++nr_token].type='+';break;
	    	case '-':tokens[++nr_token].type='-';break;
	   		 case '*':tokens[++nr_token].type='*';break;
	    	case '/':tokens[++nr_token].type='/';break;
	    	 case '(':tokens[++nr_token].type='(';break;
	    	case ')':tokens[++nr_token].type=')';break;
	    	case TK_EQ:tokens[++nr_token].type=TK_EQ;break;
	    	case TK_UEQ:tokens[++nr_token].type=TK_UEQ;break;
	    	case TK_AND:tokens[++nr_token].type=TK_AND;break;
	    	case TK_REG:tokens[++nr_token].type=TK_REG;
	    	for(int i=0;i<substr_len;++i)
	   			tokens[nr_token].str[i]=*(substr_start+i);
		     	tokens[nr_token].str[substr_len]='\0';
				
	    	break;
	    	case TK_NUM:tokens[++nr_token].type=TK_NUM;
			for(int i=0;i<substr_len;++i)
	   			tokens[nr_token].str[i]=*(substr_start+i);
		     	tokens[nr_token].str[substr_len]='\0';
				
	    	break;
	    	case TK_HNUM:tokens[++nr_token].type=TK_HNUM;
			for(int i=0;i<substr_len;++i)
	   			tokens[nr_token].str[i]=*(substr_start+i);
		     	tokens[nr_token].str[substr_len]='\0';
				
	    	break;
	    	case TK_NOTYPE:break;
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
bool okay=0;
bool check_parentheses(int p,int q){
	if((tokens[p].type!='(')||(tokens[q].type!=')'))
		return 0;
	int sum=0;
	for(int i=p+1;i<=q-1;++i){
		if(tokens[i].type=='(') ++sum;
		if(tokens[i].type==')') --sum;
		if(sum<0) return 0;
	}
	if(sum!=0) return 0;
	return 1;
}
int eval(int p, int q) {
  //printf("%d %d\n",p,q);
  if (p > q) {
    /* Bad expression */
    okay=0;return 0;
  }
  else if (p == q) {
	if(tokens[p].type==TK_NUM){
		int len=strlen(tokens[p].str),v=0;
		for(int i=0;i<len;++i)
			v=v*10+tokens[p].str[i]-'0';
		return v;
	}
	else{
		okay=0;return 0;
	}
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
  	//printf("yes\n");
  	if(tokens[p].type=='-'&&tokens[p+1].type=='-') return eval(p+2,q);
  	if((tokens[p].type=='-')&&(check_parentheses(p+1, q) == true)&&q>p+1)
  		return -eval(p+2,q-1);
  	if((tokens[p].type=='*')&&(check_parentheses(p+1, q) == true)&&q>p+1)
  		return vaddr_read(eval(p+2,q-1),4);
  	if((tokens[p].type=='-')&&p==q-1) return -eval(q,q);
  	if((tokens[p].type=='*')&&p==q-1) return vaddr_read(eval(q,q),4);
  	int op=0;
  	//printf("yes\n");
  	int sum=0;
  	for(int i=q;i>p;--i){
  		if(tokens[i].type==TK_AND){
  			if(sum==0){
  				op=i;
  				break;
  			}
  		}
  		if(tokens[i].type=='('){
  			++sum;
  		}
  		if(tokens[i].type==')'){
  			--sum;
  		}
  	}
  	sum=0;
  	for(int i=q;i>p;--i){
  		if(op) break;
  		if(tokens[i].type==TK_UEQ){
  			if(sum==0){
  				op=i;
  				break;
  			}
  		}
  		if(tokens[i].type=='('){
  			++sum;
  		}
  		if(tokens[i].type==')'){
  			--sum;
  		}
  	}
  	sum=0;
  	for(int i=q;i>p;--i){
  		if(op) break;
  		if(tokens[i].type==TK_EQ){
  			if(sum==0){
  				op=i;
  				break;
  			}
  		}
  		if(tokens[i].type=='('){
  			++sum;
  		}
  		if(tokens[i].type==')'){
  			--sum;
  		}
  	}
  	sum=0;
  	for(int i=q;i>p;--i){
  		if(op) break;
  		if((tokens[i].type=='-'&&tokens[i-1].type!='-'&&tokens[i-1].type!='+'&&tokens[i-1].type!='*'&&tokens[i-1].type!='/')||(tokens[i].type=='+')){
  			if(sum==0){
  				op=i;
  				break;
  			}
  		}
  		if(tokens[i].type=='('){
  			++sum;
  		}
  		if(tokens[i].type==')'){
  			--sum;
  		}
  	}
  	sum=0;
  	if(!op){
  		for(int i=q;i>p;--i){
  			if(tokens[i].type=='/'||(tokens[i].type=='*'&&tokens[i-1].type!='-'&&tokens[i-1].type!='+'&&tokens[i-1].type!='*'&&tokens[i-1].type!='/')){
  				if(sum==0){op=i;break;}
  			}
  			if(tokens[i].type=='('){
  				++sum;
  			}
  			if(tokens[i].type==')'){
  				--sum;
  			}
  		}
  	}
  	if(!op){
  		okay=0;
  		return 0;
    }
    int val1;
    if(tokens[p].type=='-'){
    	val1=-eval(p+1,op-1);
    }
    else val1=eval(p,op-1);
    int val2 = eval(op + 1, q);
    //printf("%d %d %d %d\n",p,q,val1,val2);
    switch (tokens[op].type) {
   	  case TK_AND:return val1&&val2;
   	  case TK_EQ:return val1==val2;
   	  case TK_UEQ:return val1!=val2;
      case '+': return val1 + val2;
      case '-': return val1-val2;
      case '*': return val1*val2;
      case '/': return val1/val2;
      default: okay=0;
     }
     return 0;
    /* We should do more things here. */
  }
}
uint32_t expr(char *e, bool *success) {
  //printf("%s\n",e);
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  okay=1;//printf("yes\n");
  for(int i=1;i<=nr_token;++i){
  	if(tokens[i].type==TK_REG){
  		tokens[i].type=TK_NUM;
  		bool ok=1;
  		int ans=isa_reg_str2val(tokens[i].str,&ok);
  		//printf("%d\n",ans);
  		if(!ok){
  			*success=false;
  			return 0;
  		}
  		//printf("%d\n",ans);
  		sprintf(tokens[i].str,"%d",ans);
  	}
  	if(tokens[i].type==TK_HNUM){
  		tokens[i].type=TK_NUM;
  		int ans=0,len=strlen(tokens[i].str);
  		for(int j=2;j<len;++j){
  			char c=tokens[i].str[j];
  			//printf("**%c\n",c);
  			if(c>='0'&&c<='9')
  				ans=ans*16+c-'0';
  			else
  				if(c>='A'&&c<='F')
  					ans=ans*16+c-'A'+9;
  				else
  					if(c>='a'&&c<='f')
  						ans=ans*16+c-'a'+9;
  					else{
  						*success=0;
  						return 0;
  					}
   		}
  		//printf("%d\n",ans);
  		sprintf(tokens[i].str,"%d",ans);
  	}
  }
  uint32_t x=eval(1,nr_token);
  if(okay) return x;
  else{
  	*success=false;
  	return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
