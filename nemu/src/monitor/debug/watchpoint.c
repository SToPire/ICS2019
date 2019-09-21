#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
	if(free_==NULL){
		printf("No free space for watchpoints!\n");
		assert(0);
	}
	if(head==NULL){

		head=free_;
		free_=free_->next;
		head->next=NULL;
		
		  WP* test=NULL;
	for(test=head;test!=NULL;test=head->next){
		printf("NO:%d val:%u",test->NO,test->val);
	}
		return head;
	}
	else{
		WP* tmp=head;
		while(tmp->next != NULL) tmp=tmp->next;
		tmp->next=free_;
		free_=free_->next;
		tmp->next->next=NULL;
		
		 /* WP* test=NULL;
	for(test=head;test!=NULL;test=head->next){
		printf("NO:%d val:%u",test->NO,test->val);
	}*/
		return tmp->next;
	}	
}

void free_wp(WP* wp){
	if(wp==head){
		head=wp->next;
	}
	else{
		WP* tmp=head;
		while(tmp->next != wp) tmp=tmp->next;
		tmp->next=wp->next;
	}
	wp->next=free_;
	free_=wp;
}
