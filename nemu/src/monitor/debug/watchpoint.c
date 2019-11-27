#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int tot=0;
void init_wp_pool() {
  int i;
  tot=0;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp(){
	if(free_==NULL){
		assert(0);
	}
	WP *x=(*free_).next;
	(*free_).next=head;
	head=free_;
	free_=x;
	(*head).num=++tot;
	return head;
}
void free_wp(WP *wp){
	(*wp).next=free_;
	free_=wp;
}
bool check(){
	bool okay=0;
	for(WP *i=head;i!=NULL;i=(*i).next){
  		bool ok=1;
  		uint32_t ans=expr((*i).s, &ok);
  		if((*i).ans!=ans){
  			(*i).ans=ans;
  			printf("%d: %s ==%08x ==%u\n",(*i).num,(*i).s,ans,ans);
  			okay=1;
  		}
  	}
  	return okay;
 }
 void isa_watch_display(){
 	for(WP *i=head;i!=NULL;i=(*i).next){
 		bool ok=1;
  		uint32_t ans=expr((*i).s, &ok);
  		(*i).ans=ans;
  		printf("%s == %u\n",(*i).s,ans);
  	}
 }
 void delete(int num){
 	WP *x=head;
 	if(x!=NULL){
 		if((*x).num==num){
 			head=(*x).next;
 			free_wp(x);
 			return;
 		}
 	}
 	for(WP *i=(*x).next;i!=NULL;x=i,i=(*i).next){
 		if((*i).num==num){
 			(*x).next=(*i).next;
 			free_wp(i);
 			return;
 		}
  	}
 }
/* TODO: Implement the functionality of watchpoint */

