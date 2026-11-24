/*
 *  @file:          rpp.c
 *  @description:
 */
#include "array_count.h"

#include <stdio.h>

#define RC 0x0D
#define LF 0x0A

int rpp_rm_rc(FILE *in) {
  unsigned char buf[256];
  unsigned char write[256];
  unsigned char *p;
  size_t to_process;

  while ((to_process = fread(buf, 1, ARRAY_COUNT(buf), in)) > 0) {
    for (size_t i = 0; i < to_process; i++) {
      if (buf[i] != RC) {
        *p++ = buf[i];
      }
      if ((i + 1) >= to_process) {
        continue;
      }
      if (buf[i + 1] == LF) {
        continue;
      }
    }
    fwrite(write, 1, (size_t)(p - write), stdout);
  }
  return 0;
}
