#include "common.h"
#include <amdev.h>
#include <klib.h>
#include <sys/types.h>

int fs_open(const char *pathname,int flags,int mode);
ssize_t fs_read(int fd,void *buf,size_t len);
size_t fs_filesz(int fd);

size_t serial_write(const void *buf, size_t offset, size_t len) {
	for(int i=0;i<len;i++)_putc(*(char *)buf++);
  return 0;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
//	char temp[len+5];
	int key= read_key();	
	int ret=-1;
	char *s=(char *)buf;
	if(key!=0){
		if(key&0x8000){
			*s++='k';
			*s++='d';
			*s++=' ';
		}	
		else {
			*s++='k';
			*s++='u';
			*s++=' ';
		}
		strcpy(s,keyname[key&0x7fff]);
		while(*s)s++;
		*s++='\n';
		*s++='\0';
		ret = sizeof(keyname[key])+4;
	}
	else {
		*s++='t';
		*s++=' ';
		uint32_t t=uptime();
		char temp[35];
		sprintf(temp,"%d",t);
		int i=0;
		while(temp[i]){
			*s++=temp[i++];
		}
		*s++='\n';
		*s++='\0';
		ret =i+3;
	}
  return ret;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(const void *buf, size_t offset, size_t len){
//	strncpy((char *)buf,dispinfo+offset,len);
	char *s=dispinfo+offset;
	int i;
	for( i=0;i<len && *s ;i++)
		*(char *)buf++=*s++;
	return i;
}


size_t fb_write(const void *buf, size_t offset, size_t len) {
	int w=screen_width();
	int x=(offset/4)%w;
 	int y=(offset/4)/w;
	draw_rect( (uint32_t *)buf, x, y, len/4,1 );
	return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
  char s[]="WIDTH:400\nHEIGHT:300\0";
  memcpy((char *)dispinfo,s,sizeof(s));
 // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
