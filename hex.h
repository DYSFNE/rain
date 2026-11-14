/**
 * @file hex.h
 * @brief Write numbers as uppercase hexadecimal digits.
 *
 * The functions write digits only. They do not write a
 * terminating '\0'.
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
char *hex_write(char *dst, uint64_t value, int digits);

/**
 * @brief Write one byte as two hexadecimal digits.
 *
 * @param[out] dst   Buffer that receives the digits.
 * @param[in]  byte  Byte to write.
 *
 * @pre @p dst is not NULL and has space for 2 characters.
 *
 * @return Pointer to the position after the last digit written.
 */
char *hex_write_byte(char *dst, unsigned char byte);

#endif /* RAIN_HEX_H */
