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
  /* ASCII */
  C, C, C, C,  C, C, C, C,  /* NUL SOH STX ETX  EOT ENQ ACK BEL */
  C, W, N, W,  W, N, C, C,  /* BS  HT  LF  VT   FF  CR  SO  SI  */
  C, C, C, C,  C, C, C, C,  /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  C, C, C, C,  C, C, C, C,  /* CAN EM  SUB ESC  FS  GS  RS  US  */
  W, P, P, P,  S, P, P, P,  /* SPC !   "   #    $   %   &   '   */
  P, P, P, P,  P, P, P, P,  /* (   )   *   +    ,   -   .   /   */
  D, D, D, D,  D, D, D, D,  /* 0   1   2   3    4   5   6   7   */
  D, D, P, P,  P, P, P, P,  /* 8   9   :   ;    <   =   >   ?   */
  S, U, U, U,  U, U, U, U,  /* @   A   B   C    D   E   F   G   */
  U, U, U, U,  U, U, U, U,  /* H   I   J   K    L   M   N   O   */
  U, U, U, U,  U, U, U, U,  /* P   Q   R   S    T   U   V   W   */
  U, U, U, P,  P, P, P, X,  /* X   Y   Z   [    \   ]   ^   _   */
  S, L, L, L,  L, L, L, L,  /* `   a   b   c    d   e   f   g   */
  L, L, L, L,  L, L, L, L,  /* h   i   j   k    l   m   n   o   */
  L, L, L, L,  L, L, L, L,  /* p   q   r   s    t   u   v   w   */
  L, L, L, P,  P, P, P, C,  /* x   y   z   {    |   }   ~   DEL */

  /* Not ASCII. All entries are R_STANDIN. */
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
  S, S, S, S,  S, S, S, S,
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
