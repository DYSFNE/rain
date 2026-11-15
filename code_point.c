#include "code_point.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hex.h"
#include "put_str.h"
#include "utf8.h"

enum { CP_BUF_SIZE = 128, CP_LINE_SIZE = 128 };

/*
**  Write one line to out that reports an invalid byte and its offset
**  in the input.  The line has the same layout as the code point lines,
**  so the output stays aligned.
**
**  Return 0 on success, or -1 if the write fails.
*/
static int print_invalid_byte(FILE *out, uint64_t offset, unsigned char byte) {
  char line[CP_LINE_SIZE];
  char *p = line;

  p = PUT_STR(p, "   0x");
  p = hex_write(p, offset, 16);
  p = PUT_STR(p, "   Invalid byte 0x");
  p = hex_write(p, byte, 2);
  *p++ = '\n';

  size_t n = (size_t)(p - line);

  return fwrite(line, 1, n, out) == n ? 0 : -1;
}

/*
**  Implementation of code_point().  See code_point.h for the interface.
**
**  The input is read in blocks into buf.  If a block ends in the middle
**  of a character, the remaining bytes are moved to the start of buf and
**  the next read continues after them.  At end of file, a character that
**  is not complete is reported as one invalid lead byte, and decoding
**  continues at the next byte.
*/
int code_point(FILE *in, FILE *out) {
  unsigned char buf[CP_BUF_SIZE];
  char line[CP_LINE_SIZE];
  size_t available = 0;
  uint64_t offset = 0;
  size_t got;

  while ((got = fread(buf + available, 1, sizeof(buf) - available, in)) > 0) {
    available += got;
    size_t total = available;
    size_t pos = 0;

    while (pos < total) {
      size_t len = utf8_lead_len(buf[pos]);

      if (len == 0) {
        if (print_invalid_byte(out, offset, buf[pos]) < 0)
          return -1;

        len = 1;
      } else if (len > available) {
        memmove(buf, buf + pos, available);
        break;
      } else {
        uint32_t cp;

        if (utf8_decode(buf + pos, len, &cp) != 0) {
          if (print_invalid_byte(out, offset, buf[pos]) < 0)
            return -1;

          len = 1;
        } else {
          char *p = line;

          p = PUT_STR(p, "   0x");
          p = hex_write(p, offset, 16);
          p = PUT_STR(p, "   U+");
          p = hex_write(p, cp, utf8_cp_len(cp));
          *p++ = '\n';

          size_t n = (size_t)(p - line);

          if (fwrite(line, 1, n, out) != n)
            return -1;
        }
      }

      pos += len;
      offset += len;
      available -= len;
    }
  }

  if (ferror(in))
    return -1;

  for (size_t i = 0; i < available; i++) {
    if (print_invalid_byte(out, offset + i, buf[i]) < 0)
      return -1;
  }

  return ferror(out) ? -1 : 0;
}
