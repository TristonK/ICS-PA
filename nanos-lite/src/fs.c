#include "fs.h"
#include <sys/types.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(const void *buf,size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

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
  {"stdout", 0, 0,0, invalid_read, invalid_write},
  {"stderr", 0, 0,0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
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
	if(ret==-1){printf("no such file\n");assert(0);}
	return ret;
}

ssize_t fs_read(int fd,void *buf, size_t len){
//	int i=fd;
	len=len+file_table[fd].open_offset<=file_table[fd].size?len:file_table[fd].size-file_table[fd].open_offset;
   // Log("len is %d",len);
//	Log("disk+open is %d",file_table[fd].disk_offset+file_table[fd].open_offset);
	ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
	file_table[fd].open_offset+=len;
	if(file_table[fd].open_offset>file_table[fd].size) file_table[fd].open_offset=file_table[fd].size;
	return len;
}

ssize_t fs_write(int fd, const void *buf, size_t len){
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
	    case SEEK_END:file_table[fd].open_offset=file_table[fd].size+offset;break;
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
