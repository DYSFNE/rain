/**
 *  @file           utf8.c
 *  @description:   Implements UTF-8 sequence classification,
 *                  validation, and decoding (see: RFC 3629).
 */

#include "utf8.h"

/*  Mask for the payload bits of the lead byte, by sequence length */
static const unsigned char lead_mask[] = {
    0x00, /*  < [0] not used: no character has 0 bytes   */
    0x7F, /*  < [1] 0xxxxxxx: keeps 7 bits   (0111 1111) */
    0x1F, /*  < [2] 110xxxxx: keeps 5 bits   (0001 1111) */
    0x0F, /*  < [3] 1110xxxx: keeps 4 bits   (0000 1111) */
    0x07, /*  < [4] 11110xxx: keeps 3 bits   (0000 0111) */
};

/*
 *  @function:      utf8_lead_len()
 *  @description:   Get the sequence length from a lead byte.
 *
 *  @lead:          First byte of a possible UTF-8 sequence.
 *
 *  @return:        Length of the sequence (1-4), or 0 if @lead cannot
 *                  start a valid sequence.
 *
 *  @note:          Bytes 0x80 - 0xBF are continuation bytes, 0xC0-0xC1
 *                  can only start overlong forms, and 0xF5-0xFF can only
 *                  start values above U+10FFFF.
 */
size_t utf8_lead_len(unsigned char lead) {
  if (lead <= 0x7F)
    return 1;

  if (lead >= 0xC2 && lead <= 0xDF)
    return 2;

  if (lead >= 0xE0 && lead <= 0xEF)
    return 3;

  if (lead >= 0xF0 && lead <= 0xF4)
    return 4;

  return 0;
}

/*
 *  @function:      utf8_cp_len()
 *  @description:   Determines the number of hexadecimal digits needed
 *                  to represent a Unicode code point, with a minimum
 *                  width of four digits.
 *
 *  @cp:            Unicode code point (0x0000-0x10FFFF).
 *
 *  @return:        4, 5, or 6 hexadecimal digits.
 *
 *  @note:          This is the display width of a code point, not
 *                  the number of bytes in its UTF-8 encoding.
 */
int utf8_cp_len(uint32_t cp) {
  int len = 4;

  if (cp > 0xFFFFF) {
    len = 6;
  } else if (cp > 0xFFFF) {
    len = 5;
  }

  return len;
}

/*
 *  @function:      utf8_is_valid_seq()
 *  @description:   Check that a buffer holds a valid UTF-8 sequence.
 *                  Rejects overlong forms, surrogates (U+D800-U+DFFF)
 *                  and values above U+10FFFF.
 *
 *  @buf:           Bytes to check. Must not be NULL and holds at least
 *                  @len bytes.
 *  @len:           Number of bytes in the sequence.
 *
 *  @return:        0 if buf[0..len - 1] is one complete, valid sequence.
 *                  -1 of the sequence is not valid.
 */
int utf8_is_valid_seq(const unsigned char *buf, size_t len) {
  if (len == 0 || len > 4 || utf8_lead_len(buf[0]) != len)
    return 0;

  /* Each continuation byte must be 10xxxxxx. */
  for (size_t i = 1; i < len; i++) {
    if ((buf[i] & 0xC0) != 0x80)
      return 0;
  }

  if (len == 1)
    return 1;

  /* For these four lead bytes, the second byte has a smaller range */
  if (buf[0] == 0xE0 && buf[1] < 0xA0)
    return 0;

  if (buf[0] == 0xED && buf[1] > 0x9F) /* surrogates: U+D800-U+DFFF */
    return 0;

  if (buf[0] == 0xF0 && buf[1] < 0x90) /* overlong: below U+10000 */
    return 0;

  if (buf[0] == 0xF4 && buf[1] > 0x8F) /* above U+10FFFF */
    return 0;

  return 1;
}

/*
 * @function:       utf8_decode()
 * @description:    Decode one UTF-8 sequence to a code point. Validates
 *                  the sequence with utf8_is_valid_seq() before decoding.
 *
 * @buf:            Bytes of the sequence. Must not be NULL and holds at
 *                  least @len bytes.
 * @len:            Number of bytes in the sequence
 * @cp:             Receives the code point on success. Must not be NULL.
 *
 * @return:         0 Success. *cp holds the code point.
 *                  -1 The sequence is not valid. *cp is not changed.
 */
int utf8_decode(const unsigned char *buf, size_t len, uint32_t *cp) {
  if (!utf8_is_valid_seq(buf, len))
    return -1;

  uint32_t value = buf[0] & lead_mask[len];

  for (size_t i = 1; i < len; i++) {
    value = (value << 6) | (buf[i] & 0x3F);
  }

  *cp = value;
  return 0;
}
