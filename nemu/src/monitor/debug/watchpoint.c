#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int head_len=0;

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
	++head_len;
	if(head==NULL){
		head=free_;
		free_=free_->next;
		head->next=NULL;
		head->NO=head_len;
		return head;
	}
	else{
		WP* tmp=head;
		while(tmp->next != NULL) tmp=tmp->next;
		tmp->next=free_;
		free_=free_->next;
		tmp->next->next=NULL;
		tmp->next->NO=head_len;
		return tmp->next;
	}	
}

void free_wp(int n){
	if(!n){printf("Illegal watchpoint number.\n");return;}
	if(head==NULL){printf("No watchpoints.\n");return;}
	WP* wp=head;
	while(wp != NULL && wp->NO != n) wp=wp->next;
	if(wp==NULL){printf("Illegal watchpoint number.\n");return;}
	--head_len;
	if(wp==head){
		head=wp->next;
	}
	else{
		WP* tmp=head;
		while(tmp->next != wp) tmp=tmp->next;
		tmp->next=wp->next;
	}
	wp->next=free_;
	wp->val=0;
	memset(wp->what,'\0',128);
	free_=wp;
	printf("Watchpoint %d has been successfully deleted.\n",n);
}
void WP_disp(){
	if(!head_len) {printf("No watchpoints.\n"); return;}
	printf("%-12s %-32s %-8s\n","Watchpoint","What","Value");
	WP *ip=head;
	while(ip!=NULL){
		printf("%-12d %-32s %-8d\n",ip->NO,ip->what,ip->val);
		ip=ip->next;
	}	
}
bool WP_check(){
	WP* tmp=head;
	bool changed=false;
	while(tmp != NULL){
		bool succ;
		uint32_t newVal=expr(tmp->what,&succ);
		if(tmp->val != newVal){
			printf("Watchpoints %d:%s changed.\nNew value:%u Old value:%u\n",tmp->NO,tmp->what,newVal,tmp->val);
			tmp->val=newVal;
			changed=true;
		}
		tmp=tmp->next;
	}
	return changed;
}
