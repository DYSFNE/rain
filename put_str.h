/**
 * @file put_str.h
 * @brief Macro to copy a string literal into a buffer.
 */

#ifndef RAIN_PUT_STR_H
#define RAIN_PUT_STR_H

#include <string.h>

/**
 * @brief Copy a string literal into a buffer, without its '\0'.
 *
 * @param ptr  Destination. Evaluated one time.
 * @param lit  A string literal. Anything else is a compile error.
 *
 * @pre @p ptr is not NULL and has space for strlen(@p lit) characters.
 *
 * @return Pointer (char *) to the position after the last character
 *         written.
 */
#define PUT_STR(ptr, lit)                                                      \
  ((char *)memcpy((ptr), "" lit "", sizeof(lit) - 1) + (sizeof(lit) - 1))

#endif /* RAIN_PUT_STR_H */
