/*
 *  @file: charclass.h
 *  @description: This file defines the interface the character class
 *  table. The table is defined in @file charclass.c
 */

#ifndef RAIN_CHARCLASS_H
#define RAIN_CHARCLASS_H

/*
 *  Defines the character classes.
 *  R_STANDIN must be 0. Bytes that are in no other class get this value.
 *  R_STANDIN does not mean that a byte is invalid.
 */
enum {
  R_STANDIN = 0, /* in no other classes: $ @ ` and 0x80-0xFF */
  R_DIGIT,       /* decimal digit: 0-9 */
  R_LOWERCASE,   /* lowercase letter: a-z */
  R_UPPERCASE,   /* uppercase letter: A-Z */
  R_WHITESPACE,  /* space, horizontal tab, vertical tab, form feed */
  R_NEWLINE,     /* line feed and carriage return */
  R_UNDERSCORE,  /* underscore: _ */
  R_PUNC,        /* punctuation */
  R_CONTROL      /* other control characters, and DEL */
};

extern const unsigned char charclass[256];

#endif /* RAIN_CHARCLASS_H */
