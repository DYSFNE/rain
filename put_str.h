/*
**  November 14, 2026
**
**  Copyright (C) 2026 DYSFNE <DYSFNE@gmail.com>
**  SPDX-License-Identifier: 0BSD
**
*************************************************************************
**  This header file defines a macro that copies a string literal into a
**  buffer without a terminating '\0'.
*/

#ifndef RAIN_PUT_STR_H
#define RAIN_PUT_STR_H

#include <string.h>

/*
**  Copy a string literal, lit, into a buffer, dst, without a terminating
**  '\0'.  dst is evaluated once.  dst must not be NULL and must have
**  space for strlen(lit) characters.
**
**  Return a char * to the position after the last character written into
**  dst.
*/
#define PUT_STR(dst, lit)                                                      \
  ((char *)memcpy((dst), "" lit "", sizeof(lit) - 1) + (sizeof(lit) - 1))

#endif /* RAIN_PUT_STR_H */
