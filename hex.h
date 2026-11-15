/*
**  2026 November 15
**
**  Copyright (C) 2026 DYSFNE <DYSFNE@gmail.com>
**  SPDX-License-Identifier: 0BSD
**
*************************************************************************
**  This header file defines the interface to the .  It
**  reads UTF-8 input and prints the Unicode code point of each character
**  on its own line.
*/

#ifndef RAIN_HEX_H
#define RAIN_HEX_H

#include <stdint.h>

/**
 * @brief Write the low digits of a value as hexadecimal.
 *
 * Writes exactly @p digits characters, most significant digit
 * first, with leading zeros. Bits of @p value above the
 * @p digits lowest hex digits are ignored.
 *
 * @param[out] dst     Buffer that receives the digits.
 * @param[in]  value   Value to write.
 * @param[in]  digits  Number of digits to write (1-16).
 *
 * @pre @p dst is not NULL and has space for @p digits characters.
 * @pre 1 <= @p digits <= 16 (checked with assert).
 *
 * @return Pointer to the position after the last digit written.
 */
extern char *hex_write(char *dst, uint64_t value, int digits);

#endif /* RAIN_HEX_H */
