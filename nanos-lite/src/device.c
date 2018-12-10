#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
    for(int i=0;i<len;i++){
	   _putc((char)*((char*)(buf+i)));
	}
	return len;
	// return 0;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
    int reallen=0;
	char temp[20001];
	Log("in read and len is %d",len);
    while(reallen<=len){
		memset(temp,'\0',sizeof(temp));
	    int keycode=read_key();
	//	Log("%s",keycode);
		if(keycode!=_KEY_NONE){
		   if((keycode&0x8000)==0){
			 sprintf(temp,"ku %s\n",keyname[keycode]);
     	}
		    else{
			sprintf(temp,"kd %s\n",keyname[keycode&0x7ff]);
		}}
	    else{
			uint32_t time=uptime();
			sprintf(temp,"t %d\n",time);
		    }
		int newlen=strlen(temp);
		if(newlen+reallen<len){
			sprintf(buf+reallen,"%s",temp);
			reallen+=newlen;
		}
		else
			break;
//		Log("now len is %d",reallen);
	}
	return reallen;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
   memcpy(buf,offset+dispinfo,len);
  // printf("%d\n",len);
  // printf("%s\n",(char *)buf);
  // Log("%s",buf);
	return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
    offset/=4;len/=4;
	int x=offset%screen_width();
	int y=offset/screen_width();
	draw_rect((uint32_t *)buf,x,y,len,1);
	 return len*4;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
  //Log("%d %d",screen_width(),screen_height());
  // sprintf(dispinfo,"WIDTH:400\nHEIGHT:300\0");
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\0",screen_width(),screen_height());
  //Log("%s",dispinfo);
   // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
