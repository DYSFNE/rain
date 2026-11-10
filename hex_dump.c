/*
 * Copyright (c) 2026 DYSFNE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "array_count.h"
#include "hex.h"
#include <stdint.h>
#include <stdio.h>

/*
 * A full 16-byte hex dump line requires at most 70 characters:
 *
 *      16  Hexadecimal offset (64-bit)
 *       3  Spaces seperating the offset from the hex values
 *      32  Hexadecimal digits (16 bytes * 2 digits)
 *      15  Spaces seperating consecutive bytes
 *       3  Additional spaces separating groups of four bytes
 *       1  Newline character
 *      --
 *      70  Total characters
 *
 * The output buffer is sized to accommodate the maximum line length
 * No null terminator is required because output is written with fwrite()
 *
 */

uint64_t hex_dump(FILE *fptr) {
  unsigned char buf[16];
  char line[96];
  size_t got;
  uint64_t offset = 0;

  while ((got = fread(buf, 1, ARRAY_COUNT(buf), fptr)) > 0) {
    char *ptrline = hex_offset(ptrline, offset, 8);
    ptrline = PUT_STR(ptrline, "   ");

    for (size_t i = 0; i < got; i++) {
      ptrline = hex_byte(ptrline, buf[i]);

      if ((i + 1) < got) {
        *ptrline++ = ' ';
        if ((i + 1) % 4 == 0) {
          *ptrline++ = ' ';
        }
      }
    }

    *ptrline++ = '\n';
    fwrite(line, 1, (size_t)(p - line), stdout);
    offset += got;
  }

  return offset;
}

int main(void) {
  FILE *fptr;
  int byte;
  int n = 0;

  fptr = fopen("setoptions.txt", "rb");

  if (fptr == NULL) {
    printf("There was an issue\n");
    return 1;
  }

  hex_dump(fptr);

  fclose(fptr);

  return 0;
}
