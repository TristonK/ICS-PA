#include "fs.h"
#include "common.h"
#include <sys/types.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);

int fs_open(const char *pathname,int flags,int mode);

size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

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
  {"stdin", 0,0, 0, invalid_read, invalid_write},
  {"stdout", 0,0, 0, invalid_read, serial_write},
  {"stderr", 0,0, 0, invalid_read, serial_write},

#include "files.h"
  {"/dev/fb", 0,0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 128,0, 0, dispinfo_read, invalid_write},
  {"/dev/events", 100,0, 0, events_read, invalid_write},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
	int t= fs_open("/dev/fb",0,0);
//	file_table[t].size=screen_width()*screen_height()*32;
	file_table[t].size=400*300*4;
/*	for(int i=0;file_table[i]!=NUL;i++){
		if(strcmp(file_table[i].name,"/dev/fb")==0){
			file_table[i].size=400*300;
			return ;
		}
	}
	*/
}

int fs_open(const char *pathname,int flags,int mode){
	int ret=-1;
 	for(int i=0;i<NR_FILES;i++)
		if(strcmp(pathname,file_table[i].name)==0){
	//		printf("%s %d\n",pathname,i);
			ret=i;
			break;
		}
	if(ret==-1){printf(pathname);printf("\n");panic("no such file in fs_open");}
	return ret;
 }

//特殊文件的offset需在fs中控制,在此加了对/bin/events的特判
ssize_t fs_read(int fd,void *buf,size_t len){
//	printf("%s %d\n",file_table[fd].name,file_table[fd].open_offset);
	if(fd>=NR_FILES ) panic("no such file in fs_read");

	ssize_t ret=0;
	if(file_table[fd].open_offset+len > file_table[fd].size)
		len=file_table[fd].size-file_table[fd].open_offset;

	if(file_table[fd].read!=NULL)
		ret= file_table[fd].read(buf, file_table[fd].open_offset,len);
	
	else 	ret= ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);

	file_table[fd].open_offset+=ret;
	if(file_table[fd].open_offset> file_table[fd].size)
		file_table[fd].open_offset=file_table[fd].size;

	return ret;
/*	if(file_table[fd].open_offset> file_table[fd].size){
		printf("%d %d\n",file_table[fd].open_offset,file_table[fd].size);
		printf("%s\n",file_table[fd].name);
		panic("in fs_read open_offset beyond file size :%s",file_table[fd].name);
	}
*/
//	if(strcmp(file_table[fd].name,"/bin/events")==0)
//		file_table[fd].open_offset=128;
}

//特殊文件的offset需在fs_write中控制
ssize_t fs_write(int fd,void *buf,size_t len){

	if(fd>=NR_FILES ) panic("no such file");
	ssize_t ret;
	
	if(file_table[fd].open_offset+len > file_table[fd].size)
		len=file_table[fd].size-file_table[fd].open_offset;
	
	if(file_table[fd].read!=NULL){
		size_t offset=file_table[fd].open_offset;
		ret= file_table[fd].write(buf, offset,len);
	}
//	if(file_table[fd].open_offset+len> file_table[fd].size){printf("len is to big\n");return -1;}
	else ret= ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);

	file_table[fd].open_offset+=ret;

	if(file_table[fd].open_offset> file_table[fd].size){
		file_table[fd].open_offset=file_table[fd].size;
		printf("%s\n",file_table[fd].name);
		panic("in fs_write open_offset beyond file size");
	}
	return ret;
}


int fs_close(int fd){
	return 0;
}

size_t fs_filesz(int fd){
	return file_table[fd].size;
}

off_t fs_lseek(int fd,off_t offset,int whence){
	
	if(fd>=NR_FILES ) panic("no such file");
	switch(whence){
		case SEEK_SET:/*printf("set\n");*/file_table[fd].open_offset=offset;break;
		case SEEK_CUR:/*printf("cur\n");*/file_table[fd].open_offset+=offset;break;
		case SEEK_END:/*printf("end\n");*/file_table[fd].open_offset=file_table[fd].size;break;
		default:break;
	}
//	printf("open_offset:%d\n",file_table[fd].open_offset);
//	printf("size::%d\n",file_table[fd].size);
//	printf("fd:%d\n",fd);
	if(file_table[fd].open_offset>file_table[fd].size){
		file_table[fd].open_offset=file_table[fd].size;
		printf("%s\n",file_table[fd].name);
		panic("in fs_lseek open_offset beyond file size");
	}
	return file_table[fd].open_offset;
}
	
