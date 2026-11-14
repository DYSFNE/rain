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
 *    The only octet of a "sequence" of one has the higher-order
 *    bit set to 0, the remaining 7 bits being used to encode the
 *    character number.
 *
 *    In a sequence of n bytes, given that n > 1, the MSB has the n
 *    higher-order bits set to 1, followed by a bit set to 0.
 *
 *    Example:
 *    Given a sequence of 3 bytes the MSB would be as follows:
 *            1110 xxxx xxxx xxxx xxxx xxxx
 *
 *    The remaining bits of the MSB contain bits from the number of the
 *    character to be encoded.
 *
 *    The following bytes all have the higher order bit set to 1 and following
 *    bit set to 0, leaving 6 bits in each to contain bits from the character
 *    to be encoded:
 *
 *             Char. number range  |        UTF-8 octet sequence
 *                (hexadecimal)    |              (binary)
 *             --------------------+---------------------------------------------
 *             0000 0000-0000 007F | 0xxxxxxx
 *             0000 0080-0000 07FF | 110xxxxx 10xxxxxx
 *             0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
 *             0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 *
 *    https://www.rfc-editor.org/info/rfc3629/#section-3
 *
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

size_t cp_byte_size(unsigned char byte) {
  size_t count = 0;

  /* A byte with the most significant bit set to 0 (0xxxxxxx) is a full 1-byte
   * character */
  if ((byte & 0x80) == 0) {
    return (size_t)1;
  }

  /* The number of 1 bits at the start of the first byte is the size
   * of the character: 110xxxxx = 2, 1110xxxx = 3, 11110xxx = 4 */
  for (int bit = 7; bit >= 3; bit--) {
    if ((byte >> bit) & 0x01) {
      count++;
    } else {
      break;
    }
  }

  /* One 1 bit (10xxxxxx) is a continuation byte, and five 1 bits
   * (11111xxx) or more are not valid in UTF-8. These cannot start a valid
   * character */
  if (count < 2 || count > 4) {
    return (size_t)0;
  }
  return count;
}

static const unsigned char first_mask[] = {
    0x00, /* [0] not used: no character has 0 bytes   */
    0x7F, /* [1] 0xxxxxxx: keeps 7 bits   (0111 1111) */
    0x1F, /* [2] 110xxxxx: keeps 5 bits   (0001 1111) */
    0x0F, /* [3] 1110xxxx: keeps 4 bits   (0000 1111) */
    0x07, /* [4] 11110xxx: keeps 3 bits   (0000 0111) */
};

static uint32_t cp_decode(const unsigned char *buf, size_t count) {
  uint32_t cp = buf[0] & first_mask[count];

  for (size_t i = 1; i < count; i++) {
    cp = (cp << 6) | (buf[i] & 0x3F);
  }
  return cp;
}

int cp_length(uint32_t cp) {
  int digits = 4;
  if (cp > 0xFFFF) {
    digits = 5;
  } else if (cp > 0xFFFF) {
    digits = 6;
  }
  return digits;
}

enum { CP_BUF_SIZE = 128, CP_LINE_SIZE = 128 };

static void print_invalid_byte(uint64_t offset, unsigned char byte) {
  char line[CP_LINE_SIZE];

  char *ptrline = write_hex(line, offset, 8);
  ptrline = PUT_STR(ptrline, "   Invalid byte");
  ptrline = hex_byte(ptrline, byte);
  *ptrline++ = '\n';

  fwrite(line, 1, (size_t)(ptrline - line), stdout);
}
/**
 *  @brief:
 *      Print the offset and the code point of each UTF-8 character
 *      in a file, one character each line
 *  @return:
 *      Return 0 on success, or -1 if a read error occurred
 */
int code_point(FILE *fptr) {
  unsigned char buf[CP_BUF_SIZE]; /* bytes from the file */
  char line[CP_LINE_SIZE];        /* one output line */
  size_t available = 0;           /* bytes in buf that are not processed */
  uint64_t offset = 0;            /* position in the file of buf[pos] */
  size_t got;                     /* bytes from the last fread */

  while ((got = fread(buf + available, 1, sizeof(buf) - available, fptr)) > 0) {
    available += got;
    size_t total = available;
    size_t pos = 0;

    while (pos < total) {
      size_t count = cp_byte_size(buf[pos]);

      /* The character is not complete in the buffer: move its bytes to the
       * start of the buffer, and write more bytes into the buffer */
      if (count > available) {
        memmove(buf, buf + pos, available);
        break;
      }

      if (count == 0) {
        print_invalid_byte(offset, buf[pos]);
        count = 1;
      } else {
        /* Write the offset as 8 hex digits (bits 31-0), highest digit first */
        char *ptrline = write_hex(line, offset, 8);
        ptrline = PUT_STR(ptrline, "   U+");

        uint64_t cp = cp_decode(&buf[pos], count);
        ptrline = write_hex(ptrline, cp, cp_length(cp));

        ptrline = PUT_STR(ptrline, "   ");

        if (count == 1) {
          *ptrline++ = char_set[cp];
        } else {
          ptrline = PUT_STR(ptrline, "ES");
        }

        *ptrline++ = '\n';

        fwrite(line, 1, (size_t)(ptrline - line), stdout);
      }

      pos += count;
      offset += count;
      available -= count;
    }
  }

  /* The file ended in the middle of a character: report each byte
   * that remain */
  for (size_t i = 0; i < available; i++) {
    print_invalid_byte(offset + i, buf[i]);
  }

  return ferror(fptr) ? -1 : 0;
}

int main(void) {
  FILE *fptr;
  int byte;
  int n = 0;

  fptr = fopen("code_point.c", "rb");

  if (fptr == NULL) {
    printf("There was an issue\n");
    return 1;
  }

  code_point(fptr);

  fclose(fptr);

  return 0;
}
