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
#include "ptr_str.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 *  The source character set is a multibyte character set which includes the
 *  basic source character set as a single-byte subset, consisting of the
 *  following 96 characters:
 *  a) 5 whitespace characters (space, horizontal tab, vertical tab, form feed,
 *  new-line)
 *  b) 10 digit characters from '0' to '9'
 *  c) 52 letters from 'a' to 'z' and from 'A' to 'Z'
 *  d) 29 punctuation characters: _ { } [ ] # ( ) < > % : ; . ? * + - / ^ & | ~
 *  ! = , \ " '
 */

typedef enum {
  C_INVALID,
  C_DIGIT,
  C_LOWERCASE,
  C_UPPERCASE,
  C_WHITESPACE,
  C_NEWLINE,
  C_PUNC
} CharClass;

static const unsigned char whitespace[] = {' ', '\t', '\v', '\f'};

static const unsigned char newline[] = {'\n', '\r'};

static const unsigned char punc[] = {
    '_', '{', '}', '[', ']', '#', '(', ')', '<', '>', '%', ':',  ';', '.', '?',
    '*', '+', '-', '/', '^', '&', '|', '~', '!', '=', ',', '\\', '"', '\''};

static unsigned char char_set[256];

/**
 *  @brief: initialize the char_set table with the
 *  class of each byte value
 */

static void init_char_set(void) {
  for (size_t i = 0; i < ARRAY_COUNT(whitespace); i++) {
    char_set[whitespace[i]] = C_WHITESPACE;
  }

  for (size_t i = 0; i < ARRAY_COUNT(newline); i++) {
    char_set[newline[i]] = C_NEWLINE;
  }

  for (size_t i = 0; i < ARRAY_COUNT(punc); i++) {
    char_set[punc[i]] = C_PUNC;
  }

  for (int c = '0'; c <= '9'; c++) {
    char_set[c] = C_DIGIT;
  }

  for (int c = 'a'; c <= 'z'; c++) {
    char_set[c] = C_LOWERCASE;
  }

  for (int c = 'A'; c <= 'Z'; c++) {
    char_set[c] = C_UPPERCASE;
  }
}

/**
 *  @brief: Get the size of the UTF-8 character that starts with a byte
 *  @param:
 *      byte: The first byte of the character.
 *  @return:
 *      The size of the character in bytes (1 to 4), or 0 if 'byte'
 *      cannot start a character (a continuation byte, or a byte
 *      that is not valid in UTF-8)
 */

int cp_byte_size(unsigned char byte) {
  int count = 0;

  /* A byte with the most significant bit set to 0 (0xxxxxxx) is a full 1-byte
   * character */
  if ((byte & 0x80) == 0) {
    return 1;
  }

  /*
   * The number of 1 bits at the start of the first byte is the size
   * of the character: 110xxxxx = 2, 1110xxxx = 3, 11110xxx = 4
   */
  for (int bit = 7; bit >= 3; bit--) {
    if ((byte >> bit) & 0x01) {
      count++;
    } else {
      break;
    }
  }

  /*
   * One 1 bit (10xxxxxx) is a continuation byte, and five 1 bits
   * (11111xxx) are not valid in UTF-8. These cannot start a valid character
   */
  if (count < 2 || count > 4) {
    return 0;
  }
  return count;
}

int code_point(FILE *fptr) {
  unsigned char buf[128];
  char line[128];
  size_t got; // bytes received
  size_t available = 0;
  uint64_t offset = 0;
  int count;

  while ((got = fread(buf + available, 1, (sizeof(buf) - available), fptr)) >
         0) {
    available += got;

    for (size_t i = 0; i < got; i++) {
      /* Write the offset as 8 hex digits (bits 31-0), highest digit first */
      char *ptrline = hex_offset(line, offset, 8);
      /* Add space between the hex offset and the code point table data */
      ptrline = PUT_STR(ptrline, "   ");
      /* Get the byte size of the character */
      count = cp_byte_size(buf[i]);

      if (count > available) {
        memmove(buf, buf + got - available, available);
        break;
      }

      switch (count) {
      case 1:
        ptrline = PUT_STR(ptrline, "U+00");
        ptrline = hex_byte(ptrline, buf[i]);
        offset += count;
        available -= count;
        break;
      case 2:
        if (got - i < count) {
          return 0;
        }
        ptrline = PUT_STR(ptrline, "U+");
        ptrline = hex_byte(ptrline, (buf[i] & 0x3F));     // 0x3F 0011 1111
        ptrline = hex_byte(ptrline, (buf[i + 1] & 0x7F)); // 0x7F 0111 1111
        offset += count;
        available -= count;
        break;
      case 3:
        ptrline = PUT_STR(ptrline, "U+");
        ptrline = hex_byte(ptrline, (buf[i] & 0x1F));     // 0x1F 0001 1111
        ptrline = hex_byte(ptrline, (buf[i + 1] & 0x7F)); // 0x7F 0111 1111
        ptrline = hex_byte(ptrline, (buf[i + 2] & 0x7F)); // 0x7F 0111 1111
        offset += count;
        available -= count;
        break;
      case 4:
        ptrline = PUT_STR(ptrline, "U+");
        ptrline = hex_byte(ptrline, (buf[i] & 0x0F));     // 0x1F 0000 1111
        ptrline = hex_byte(ptrline, (buf[i + 1] & 0x7F)); // 0x7F 0111 1111
        ptrline = hex_byte(ptrline, (buf[i + 2] & 0x7F)); // 0x7F 0111 1111
        ptrline = hex_byte(ptrline, (buf[i + 3] & 0x7F)); // 0x7F 0111 1111
        offset += count;
        available -= count;
        break;
      }
    }
  }
  return 0;
}

int main(void) {
  FILE *fptr;
  int byte;
  int n = 0;

  fptr = fopen("phase_1.c", "rb");

  if (fptr == NULL) {
    printf("There was an issue\n");
    return 1;
  }

  code_point(fptr);

  fclose(fptr);

  return 0;
}
