#include "common.h"
/*#include <sys/types.h>
int fs_open(const char *pathname, int flags ,int mode);
fs_filesz
ssize_t fs_read(int fd, void *buf, size_t len);
ssize_t fs_write(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t fs_filesz(int fd);
*/
extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))

/* The kernel is monolithic, therefore we do not need to
 * translate the address `buf' from the user process to
 * a physical one, which is necessary for a microkernel.
 */

/* read `len' bytes starting from `offset' of ramdisk into `buf' */
size_t ramdisk_read(void *buf, size_t offset, size_t len) {
//  Log("offset is  %d and len is %d",offset,len);
   Log("RAMDISK_SIZE is %d",RAMDISK_SIZE);  
	assert(offset + len <= RAMDISK_SIZE);
  memcpy(buf, &ramdisk_start + offset, len);
  return len;
}

/* write `len' bytes starting from `buf' into the `offset' of ramdisk */
size_t ramdisk_write(const void *buf, size_t offset, size_t len) {
  assert(offset + len <= RAMDISK_SIZE);
  memcpy(&ramdisk_start + offset, buf, len);
  return len;
}

void init_ramdisk() {
  Log("ramdisk info: start = %p, end = %p, size = %d bytes",
      &ramdisk_start, &ramdisk_end, RAMDISK_SIZE);
}

size_t get_ramdisk_size() {
  return RAMDISK_SIZE;
}
