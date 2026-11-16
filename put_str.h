/*
 *  @file:          put_str.h
 *  @description:   Defines a macro for copying a string literal into a
 *                  buffer.
 */

#ifndef RAIN_PUT_STR_H
#define RAIN_PUT_STR_H

#include <string.h>

/*
 *  @macro:         PUT_STR(dst, lit)
 *  @description:   Copy a string literal into a buffer without adding a
 *                  terminating '\0'.
 *
 *  @dst:           Writable buffer with least sizeof(lit) - 1 bytes space
 *                  available.
 *  @lit:           String literal to be copied. MUST be a string literal.
 *
 *  @return:        Pointer to the position after the last byte written.
 */
#define PUT_STR(dst, lit)                                                      \
  ((char *)memcpy((dst), "" lit "", sizeof(lit) - 1) + (sizeof(lit) - 1))

#endif /* RAIN_PUT_STR_H */
