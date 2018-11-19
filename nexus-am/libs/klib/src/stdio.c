#include "klib.h"
#include <stdarg.h>

#ifndef __ISA_NATIVE__

int printf(const char *fmt, ...) {
  char pbuf[1024];
  va_list ap;
  int i;
  va_start(ap,fmt);
  i=vsprintf(pbuf,fmt,ap);
  va_end(ap);
  //puts(pbuf);
  return i;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  

  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
    va_list ap;
	int i;
	va_start(ap,fmt);
    i=vsprintf(out,fmt,ap);
	va_end(ap);
  	return i;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
