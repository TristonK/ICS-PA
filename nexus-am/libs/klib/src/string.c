#include "klib.h"

//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__)||defined(__NATIVE_USE_KLIB__)
size_t strlen(const char *s) {

	const char *t=s;
	while(*t++);
	return (size_t)(t -s -1);
}

char *strcpy(char* dst,const char* src) {
	char * start=dst;
	while(*src)
	{
		*dst=*src;
		dst++;
		src++;
	}
	*dst='\0';
  return start;
}

char* strncpy(char* dst, const char* src, size_t n) {
	size_t i;
	for(i=0;i<n &&src[i]!='\0';i++)
		dst[i]=src[i];
	for(;i<n;i++)
		dst[i]='\0';
	return dst;
}

char* strcat(char* dst, const char* src) {
	char *t=dst;
       while(*t)t++;//find the end of dst;
	while( (*t=*src) ){
		src++;
		t++;
	}
	return	dst;//src应该有终止空字符，此处不再多加，可能。。
}

int strcmp(const char* s1, const char* s2) {

	while(*s1==*s2 )
	{
		if(*s1=='\0')return 0;
		else ++s1,++s2;
	}
	if(*(unsigned char *)s1<*(unsigned char *)s2)return -1;
	else if(*(unsigned char *)s1>*(unsigned char *)s2)return 1;
	else return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	//return >0 if s1>s2
  	if(!n)return 0;
	while( n)
	{
		if(*s1==*s2)s1++,s2++;
		else break;
		n--;
	}
	if(!n)return 0;
	if(*(unsigned char *)s1>*(unsigned char *)s2)return 1;
	else return -1;
}

void* memset(void* v,int c,size_t n) {
	void *start=v;
	while(n--){
		*(char *)v=(char)c;
		(char *)v++;
	}
	return start;
}

void* memcpy(void* out, const void* in, size_t n) {
	void *ret=out;
	while(n--){
		*(char *)out=*(char *)in;
		(char *)out++;
		(char *)in++;
	}
	return ret;
}

int memcmp(const void* s1, const void* s2, size_t n){
	if(!n)return 0;
	while(--n && *(char *)s1 == *(char *)s2)
	{
		(char *)s1++;
		(char *)s2++;	
	}
	return (*(unsigned char *)s1- *(unsigned char *)s2);	
}

#endif
