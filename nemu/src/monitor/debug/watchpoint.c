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
    wp_pool[i].used=0;
    wp_pool[i].now=0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp()
{   
    if((free_)==NULL)
    {
      assert(0);
    }
    else
    {  
      WP *m1;
      m1=free_;
      if((free_->next)==NULL)
      {
	free_=NULL;
      }
      else free_=free_->next;
      if(head==NULL)
      head=m1;
      else
      {      
        m1->next=head;
	head=m1;
      }
      m1->used=1;
      return m1;
    }
} 
void free_wp(WP *wp)
{
    if(head ==NULL)
    {
      assert(0);
    }
    else
    {  
      WP *m1;
      wp->used=0;
      if(wp==head)
      {
	if(head->next==NULL)
	{
          head=NULL;
	}
      }
      else
      {
        m1=head;
        while((m1->next!=wp)&&(m1->next!=NULL)) m1=m1->next;
        if(m1->next!=wp) assert(0);
        m1->next=wp->next;
      }
      if(free_==NULL)
      free_=wp;
      else
      {       
          wp->next=free_;
	  free_=wp;
      }
    }
}
void free1(int now)
{
	if((now<32) && (now>=0)) 
	{
	  if(wp_pool[now].used==1)
	  {
	    free_wp(&wp_pool[now]);
	    printf("NOW watchpoint %d release,%d",now,wp_pool[now].used);
	  }
        }
}
void issert(char *args,uint32_t ans)
{
	WP* wp1;
	wp1=new_wp();
	strcpy(wp1->expr,args);
	wp1->now=ans;
	printf("watchpoint NO.%d used and now is %d\n",wp1->NO,wp1->now);
}
void show()
{
	for(int i=0;i<NR_WP;i++)
	{
          if(wp_pool[i].used==1)
	  {
		  printf("%d %s %d\n",wp_pool[i].NO,wp_pool[i].expr,wp_pool[i].now);
	  }
	}
}
bool check1()
{
	bool bj=false;
	for(int i=0;i<=31;i++)
	{
		if(wp_pool[i].used==1)
		{
                  bool success=true;
		  uint32_t new=expr(wp_pool[i].expr,&success);
		  if(wp_pool[i].now!=new)
		  {
			  bj=true;
			  wp_pool[i].now=new;
			  printf("watchpoint%d change to %u\n",i,new);
		  }
		}
	}
	return(bj);
}
/* TODO: Implement the functionality of watchpoint */


