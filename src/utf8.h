/*
 *  @file           utf8.h
 *  @description:   Interface for UTF-8 sequence classification,
 *                  validation, and decoding (see: RFC 3629).
 */

#ifndef RAIN_UTF8_H
#define RAIN_UTF8_H

#include <stddef.h>
#include <stdint.h>

extern size_t utf8_lead_len(unsigned char lead);
extern int utf8_is_valid_seq(const unsigned char *buf, size_t len);
extern int utf8_decode(const unsigned char *buf, size_t len, uint32_t *cp);
extern int utf8_cp_len(uint32_t cp);

#endif /* RAIN_UTF8_H */
