#include "array_count.h"
#include "hex.h"
#include "put_str.h"
#include <stdio.h>

/*
 * A 16-byte dump line requires at most 70 characters.
 * No null terminator is required
 */

int hex_dump(FILE *in, FILE *out) {
  unsigned char buf[16];
  char line[96];
  size_t n;
  uint64_t offset = 0;

  while ((n = fread(buf, 1, ARRAY_COUNT(buf), in)) > 0) {
    char *p = line;

    p = hex_write(p, offset, 16);
    p = PUT_STR(p, "   ");

    for (size_t i = 0; i < n; i++) {
      p = hex_write(p, buf[i], 2);

      if ((i + 1) < n) {
        *p++ = ' ';

        if ((i + 1) % 4 == 0) {
          *p++ = ' ';
        }
      }
    }

    *p++ = '\n';

    if (fwrite(line, 1, (size_t)(p - line), out) != (size_t)(p - line)) {
      return -1;
    }

    offset += n;
  }

  return ferror(in) ? -1 : 0;
}
