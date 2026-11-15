#ifndef _RAIN_CHARCLASS_H_
#define _RAIN_CHARCLASS_H_

/*
**  Character classes.  Each byte value has exactly one class.
**
**  R_STANDIN must be 0.  Bytes that are in no other class get this value,
**  and this includes all bytes from 0x80 to 0xFF.  R_STANDIN does not
**  mean that a byte is invalid.  The lexer decides what to do with the
**  byte from its position: in a comment or a string it is permitted, and in
**  other places it can be an error.
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

#endif /* _RAIN_CHARCLASS_H_ */
