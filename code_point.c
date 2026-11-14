#include "hex.h"
#include "put_str.h"
#include "utf8.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum { CP_BUF_SIZE = 128, CP_LINE_SIZE = 128 };

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
/**
 *  @brief:
 *      Print the offset and the code point of each UTF-8 character
 *      in a file, one character each line
 *  @return:
 *      Return 0 on success, or -1 if a read error occurred
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

          if (fwrite(line, 1, n, stdout) != n)
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

  return ferror(out) ? -1 : -0;
}

int main(void) {
  FILE *fptr;

  fptr = fopen("code_point.c", "rb");

  if (fptr == NULL) {
    printf("There was an issue\n");
    return 1;
  }

  code_point(fptr, stdout);

  fclose(fptr);

  return 0;
}
