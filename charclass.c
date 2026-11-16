/*
 *  @file:          charclass.c
 *  @description:   Maps each byte value (0x00-0xFF) to a character class.
 */

#include "charclass.h"

#define S R_STANDIN
#define D R_DIGIT
#define L R_LOWERCASE
#define U R_UPPERCASE
#define W R_WHITESPACE
#define N R_NEWLINE
#define X R_UNDERSCORE
#define P R_PUNC
#define C R_CONTROL

/* clang-format off */
const unsigned char charclass[256] = {
  /* ASCII: 0x00-0x7F */
  C, C, C, C,  C, C, C, C,  /* 0x00-0x07  NUL SOH STX ETX  EOT ENQ ACK BEL */
  C, W, N, W,  W, N, C, C,  /* 0x08-0x0F  BS  HT  LF  VT   FF  CR  SO  SI  */
  C, C, C, C,  C, C, C, C,  /* 0x10-0x17  DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  C, C, C, C,  C, C, C, C,  /* 0x18-0x1F  CAN EM  SUB ESC  FS  GS  RS  US  */
  W, P, P, P,  S, P, P, P,  /* 0x20-0x27  SPC !   "   #    $   %   &   '   */
  P, P, P, P,  P, P, P, P,  /* 0x28-0x2F  (   )   *   +    ,   -   .   /   */
  D, D, D, D,  D, D, D, D,  /* 0x30-0x37  0   1   2   3    4   5   6   7   */
  D, D, P, P,  P, P, P, P,  /* 0x38-0x3F  8   9   :   ;    <   =   >   ?   */
  S, U, U, U,  U, U, U, U,  /* 0x40-0x47  @   A   B   C    D   E   F   G   */
  U, U, U, U,  U, U, U, U,  /* 0x48-0x4F  H   I   J   K    L   M   N   O   */
  U, U, U, U,  U, U, U, U,  /* 0x50-0x57  P   Q   R   S    T   U   V   W   */
  U, U, U, P,  P, P, P, X,  /* 0x58-0x5F  X   Y   Z   [    \   ]   ^   _   */
  S, L, L, L,  L, L, L, L,  /* 0x60-0x67  `   a   b   c    d   e   f   g   */
  L, L, L, L,  L, L, L, L,  /* 0x68-0x6F  h   i   j   k    l   m   n   o   */
  L, L, L, L,  L, L, L, L,  /* 0x70-0x77  p   q   r   s    t   u   v   w   */
  L, L, L, P,  P, P, P, C,  /* 0x78-0x7F  x   y   z   {    |   }   ~   DEL */

  /* Not ASCII: 0x80-0xFF.  All entries are R_STANDIN. */
  S, S, S, S,  S, S, S, S,  /* 0x80-0x87 */
  S, S, S, S,  S, S, S, S,  /* 0x88-0x8F */
  S, S, S, S,  S, S, S, S,  /* 0x90-0x97 */
  S, S, S, S,  S, S, S, S,  /* 0x98-0x9F */
  S, S, S, S,  S, S, S, S,  /* 0xA0-0xA7 */
  S, S, S, S,  S, S, S, S,  /* 0xA8-0xAF */
  S, S, S, S,  S, S, S, S,  /* 0xB0-0xB7 */
  S, S, S, S,  S, S, S, S,  /* 0xB8-0xBF */
  S, S, S, S,  S, S, S, S,  /* 0xC0-0xC7 */
  S, S, S, S,  S, S, S, S,  /* 0xC8-0xCF */
  S, S, S, S,  S, S, S, S,  /* 0xD0-0xD7 */
  S, S, S, S,  S, S, S, S,  /* 0xD8-0xDF */
  S, S, S, S,  S, S, S, S,  /* 0xE0-0xE7 */
  S, S, S, S,  S, S, S, S,  /* 0xE8-0xEF */
  S, S, S, S,  S, S, S, S,  /* 0xF0-0xF7 */
  S, S, S, S,  S, S, S, S,  /* 0xF8-0xFF */
};
/* clang-format on */

#undef S
#undef D
#undef L
#undef U
#undef W
#undef N
#undef X
#undef P
#undef C
