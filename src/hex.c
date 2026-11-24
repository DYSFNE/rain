/*
 *  @file:          hex.c
 *  @description:   Implements a function that writes an integer as
 *                  hexadecimal digits into a char buffer.
 */

#include "hex.h"

#include <assert.h>

/*  Hexadecimal digit character map. */
static const char hexdigits[] = "0123456789ABCDEF";

/*
 *  @function:      hex_write()
 *  @description:   Writes @width digits of @value into @dst as uppercase
 *                  hexadecimal digits, without null termination.
 *
 *  @dst:           Writable buffer with @width bytes available.
 *  @value:         Integer to be converted to its hexadecimal
 *                  representation.
 *  @width:         Exactly how many digits of @value that are to be
 *                  written. Must be between 1 to 16 inclusive.
 *
 *  @return:        Pointer to the position after the last byte written.
 *
 *  @example:       @value = 0x1234 and @width = 2 writes "34";
 *                  @width = 6 writes "001234"
 */
char *hex_write(char *dst, uint64_t value, int width) {
  assert(width >= 1 && width <= 16);

  for (int shift = (width - 1) * 4; shift >= 0; shift -= 4) {
    *dst++ = hexdigits[(value >> shift) & 0x0F];
  }
  return dst;
}
