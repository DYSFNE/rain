/*
 *  @file:          charclass.h
 *  @description:   Character classification interface.
 */

#ifndef RAIN_CHARCLASS_H
#define RAIN_CHARCLASS_H

/*
 *  Character classes. Each byte belongs to exactly one class.
 *
 *  R_STANDIN must be 0. Bytes that are in no other class get this value,
 *  but does not imply an invalid byte
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
