#include "klib.h"

//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
   /* const char *sc=s;
	while(*sc!='\0'){
		sc++;
	}
  	return (sc-s);*/
	int count=0;
	while(*s){
		count++;
		s++;
	}
	return count;
}

char *strcpy(char* dst,const char* src) {
   /* char *s=dst;
	for(s=dst;(*s++=*src++)!='\0';);
	return (dst);*/
	char *s=dst;
	while((*dst++=*src++)!='\0');
	return s;
}

char* strncpy(char* dst, const char* src, size_t n) {
	char *s=dst;
	while(n--){
		if(*src!='\0')
			*dst++=*src++;
		else
			*dst++='\0';
	}
  return s;
}

char* strcat(char* dst, const char* src) {
	/*char *s;
	for(s=dst;*s!='\0';s++);
	for(;(*s=*src)!='\0';++s,src++);
    return dst;*/
	char *s=dst;
	while(*dst!='\0')
		++dst;
	while((*dst++=*src++)!='\0');
	return s;
}

int strcmp(const char* s1, const char* s2) {
	/*for(;*s1==*s2;s1++,s2++)
		if(*s1=='\0')
			return 0;
	return ((*(unsigned char *)s1<*(unsigned char *)s2)?-1:+1);*/
	while(*s1&&*s2&&*s1==*s2){
		++s1;
		++s2;
	}
	return (*s1-*s2);
}

int strncmp(const char* s1, const char* s2, size_t n) {
   //may wrong
	for(;n>0;++s1,++s2,--n)
		if(*s1!=*s2)
			return ((*(unsigned char *)s1<*(unsigned char *)s2)?-1:+1);
		else if(*s1=='\0')
		     return -1;
	return 0;
}

void* memset(void* v,int c,size_t n) {
 /* const unsigned char uc=c;
  unsigned char *su;
  for(su=v;n>0;su++,n--)
      *su=uc;
   return v;*/
   char * su=v;
   while(n--){
	   *su++=c;
   }
   return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  //changed
  void *rett=out;
  char *su1=(char *)out;
  const char *su2=(char *)in;
  for(;n>0;n--)
       *su1++=*su2++;
  return rett;
}

int memcmp(const void* s1, const void* s2, size_t n){
 /* const unsigned char *su1,*su2;
  for(su1=s1,su2=s2;n>0;su1++,su2++,n--)
	  return ((*su1<*su2)?-1:+1);
  return (0);*/
	const unsigned char *su1=s1,*su2=s2;
	while(n--){
		if(*su1++!=*su2++)
			return (*--su1-*--su2);
	}
	return 0;
}

#endif
