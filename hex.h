/*
 *  @file:          hex.h
 *  @description:   Interface for fix-width hexadecimal formatting.
 */

#ifndef RAIN_HEX_H
#define RAIN_HEX_H

#include <stdint.h>

extern char *hex_write(char *dst, uint64_t value, int width);

#endif /* RAIN_HEX_H */
