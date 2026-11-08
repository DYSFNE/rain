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

#include <stdint.h>
#include <stdio.h>

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

#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a)[0])

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

int phase_1(void) {}

int main(void) {
  FILE *fptr;
  int byte;
  int n = 0;

  fptr = fopen("phase_1.c", "rb");

  if (fptr == NULL) {
    printf("There was an issue\n");
    return 1;
  }

  phase_1(fptr);

  fclose(fptr);

  return 0;
}
