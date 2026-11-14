/**
 * @file hex.c
 * @brief Implementation of hex.h.
 */

#include "hex.h"
#include <assert.h>

/** Digit characters, indexed by the value of a 4-bit nibble. */
static const char hexdigits[] = "0123456789ABCDEF";

char *hex_write(char *dst, uint64_t value, int digits) {
  assert(digits >= 1 && digits <= 16);

  /* Start at the most significant nibble that we write, and move
   * down 4 bits for each digit. */
  for (int shift = (digits - 1) * 4; shift >= 0; shift -= 4) {
    *dst++ = hexdigits[(value >> shift) & 0x0F];
  }
  return dst;
}

char *hex_write_byte(char *dst, unsigned char byte) {
  return hex_write(dst, byte, 2);
}
