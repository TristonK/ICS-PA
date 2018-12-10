#include "fs.h"
#include <sys/types.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(const void *buf,size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf,size_t offset,size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);


typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0,0, invalid_read, invalid_write},
  {"stdout", 0, 0,0, invalid_read, serial_write},
  {"stderr", 0, 0,0, invalid_read, serial_write},
#include "files.h"
  {"/dev/fb",0,0,0,invalid_read,fb_write},
  {"/proc/dispinfo",128,0,0,dispinfo_read,invalid_write},
  {"/dev/events",0,0,0,events_read,invalid_write},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))
int fs_open(const char* pathname, int flags, int mode);
int fs_close(int fd);

void init_fs() {
  // TODO: initialize the size of /dev/fb
  int fd=fs_open("/dev/fb",0,0);
  file_table[fd].size=screen_width()*screen_height()*4;
  fs_close(fd);
}

//my func begin
int fs_open(const char* pathname, int flags, int mode){
	int ret=-1;
	for(int i=0;i<NR_FILES;i++){ 
		if(/*pathname==*file_table [i].name*/!strcmp(pathname,file_table[i].name)){
		   ret=i;
		   file_table[i].open_offset=0;
		   break;
		}
	}
	if(ret==-1){printf("%s no such file\n",pathname);assert(0);}
	return ret;
}

ssize_t fs_read(int fd,void *buf, size_t len){
//	int i=fd; 
    switch(fd){
        case FD_STDIN:
		case FD_STDOUT:
		case FD_STDERR:
			return 0;
		default:
	len=(len+file_table[fd].open_offset)<=file_table[fd].size?len:file_table[fd].size-file_table[fd].open_offset;
   // Log("len is %d",len);
//	Log("disk+open is %d",file_table[fd].disk_offset+file_table[fd].open_offset);
	if(file_table[fd].read==NULL)
	ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
	else file_table[fd].read(buf,file_table[fd].open_offset,len);
	file_table[fd].open_offset+=len;
	if(file_table[fd].open_offset>file_table[fd].size) file_table[fd].open_offset=file_table[fd].size;
	return len;}
}

ssize_t fs_write(int fd, const void *buf, size_t len){
/*	switch (fd){
		case FD_STDIN:
			return 0;
		case FD_STDOUT:
		case FD_STDERR:
			for(int i=0;i<len;i++){
			  _putc((char)*((char*)(buf+i)));
			}
			return len;
		default:*/
	if(file_table[fd].write!=NULL)
		return file_table[fd].write(buf,file_table[fd].open_offset,len);
     len=len+file_table[fd].open_offset<=file_table[fd].size?len:file_table[fd].size-file_table[fd].open_offset;
	 ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
	 file_table[fd].open_offset+=len;
		if(file_table[fd].open_offset>file_table[fd].size) file_table[fd].open_offset=file_table[fd].size;
	 return len;
}

off_t fs_lseek(int fd, off_t offset, int whence){
	switch (whence){
	    case SEEK_SET:file_table[fd].open_offset=offset;break;
	    case SEEK_CUR:file_table[fd].open_offset=file_table[fd].open_offset+offset;break;
	    case SEEK_END:file_table[fd].open_offset=file_table[fd].size/*+offset*/;break;
		default:
					  panic("no such op of fs_lseek");
	}
	if(file_table[fd].open_offset>file_table[fd].size) file_table[fd].open_offset=file_table[fd].size;
	return file_table[fd].open_offset;
}

int fs_close(int fd){
	return 0;
}

size_t fs_filesz(int fd){
 return file_table[fd].size;
}
//my func end
