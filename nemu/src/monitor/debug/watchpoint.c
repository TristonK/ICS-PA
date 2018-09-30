#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
	wp_pool[i].used=false;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_up(char *args){
	WP *temp;//,*headco;
	//headco=head;
	if(free_==NULL){
		printf("no free watchpoints\n");
		assert(0);
		}
	temp=free_;
	strcpy(temp->exp,args);
	bool succ=1;
	uint32_t val=expr(temp->exp,&succ);
	if(!succ){
		printf("wrong input");
		assert(0);
		}
	temp->next=head;
	temp->val=val;
	temp->used=true;
	free_=free_->next;
	head=temp;
	/*if(head==NULL){
		head=temp;
		head->next=NULL;
		}
	else {
		while(headco->next!=NULL)
			headco=headco->next;
		headco->next=temp;
		}*/
	return temp;
	}
void free_wp(WP *wp){
	//WP *temp;
//	temp=free_;
	wp->used=false;
	wp->next=free_;
	memset(wp->exp,'\0',sizeof(wp->exp));
	free_=wp;
	/*if(free_==NULL){
		free_=wp;
		}
    else{
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next
		}*/
	}


	
