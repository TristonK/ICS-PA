#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
void free_wp(WP *wp);
WP* new_wp();
int cnt=1;
void init_wp_pool() {
	for(int j=0;j<NR_WP;j++){
	memset(wp_pool[j].exp,'\0',sizeof(wp_pool[j].exp));
	}
	cnt=1;
  int i;
  wp_pool[0].p=&wp_pool[NR_WP-1];
  wp_pool[NR_WP-1].next=&wp_pool[0];
  wp_pool[NR_WP-1].p=&wp_pool[NR_WP-2];
  wp_pool[0].next=&wp_pool[1];
 wp_pool[NR_WP-1].NO=NR_WP-1;
   wp_pool[0].NO=1;
   wp_pool[0].hit_time=wp_pool[NR_WP-1].hit_time=0; 
  for (i = 1; i < NR_WP-1; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].p=&wp_pool[i-1];
    wp_pool[i].hit_time=0;
  }
 // wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
	if(free_==head){printf("the pool is full\n");assert(0);}
	WP* new=free_;
	if(head==NULL)head=free_;
	new->NO=cnt++;
	free_=free_->next;
	new->hit_time=1;	
	return new;	
}
void free_wp(WP *wp){
	if(head==NULL){printf("pool is empty\n");return ;}
	WP * temp=head;
	if(head==wp){
		if(head->next==free_){	free_=head;head=NULL;	}
		else head=head->next;
		return ;
	}
	while(temp!=wp){
		if(temp==free_){printf("no need to free\n");return ;}
		temp=temp->next;
	}
	temp->p->next=temp->next;
	temp->next->p=temp->p;
	temp->p=head->p;
	head->p->next=temp;
	temp->next=head;
	head->p=temp;
	if(head==free_)free_=temp;
	return ;
}
bool check_watchpoint(){
	WP *ptr=head;
	bool flag=0;
	if(head==NULL)flag=0;
	else {
		while(ptr!=free_){
			bool success=1;
			uint32_t newans=expr(ptr->exp,&success);    
			if(success==false){
			printf("error to calculate expr in check_watchpoint\n");flag=1;
			}
			else 	if(newans!=ptr->oldvalue){
			    printf("watchpoint %d: %s\n",ptr->NO,ptr->exp); 
			    printf("Old value = %u\n New value = %u\n",ptr->oldvalue,newans);flag=1;
			    ptr->hit_time++;
			    printf("hit time: %d\n",ptr->hit_time);
			ptr->oldvalue=newans;
			}
			ptr=ptr->next;
		}
	}
	return flag;
}
void info_w(){
	WP *ptr=head;
	if(head==NULL)printf("No watchpoints.\n");
	else {
		printf("Num\t What\n");

		if(head==free_){
			printf("%d\t  %s\n",ptr->NO,ptr->exp);
		ptr=ptr->next;		
		}
		while(ptr!=free_){
		printf("%d\t %s\n",ptr->NO,ptr->exp);
		printf("\t hit times:%d\n",ptr->hit_time);
		ptr=ptr->next;
		}
	}
}
void delete_wp(int n){
	if(head==NULL){printf("no watchpoint\n");return ; }
	WP *ptr=head;
	if(head->NO==n){
		free_wp(ptr);
		printf("delete the watchpoint: %d\n",n);
		return ;
	}
	while((ptr->NO!=n) &&(ptr!=free_)){
		ptr=ptr->next;
	}
	free_wp(ptr);
	printf("delete the watchpoint: %d\n",n);
	return ;
}
