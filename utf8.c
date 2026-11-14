/**
 * @file utf8.c
 * @brief Implementation of utf8.h
 */

#include "utf8.h"

/** Mask for the payload bits of the lead byte, by sequence length */
static const unsigned char lead_mask[] = {
    0x00, /**< [0] not used: no character has 0 bytes   */
    0x7F, /**< [1] 0xxxxxxx: keeps 7 bits   (0111 1111) */
    0x1F, /**< [2] 110xxxxx: keeps 5 bits   (0001 1111) */
    0x0F, /**< [3] 1110xxxx: keeps 4 bits   (0000 1111) */
    0x07, /**< [4] 11110xxx: keeps 3 bits   (0000 0111) */
};

size_t utf8_lead_len(unsigned char lead) {
  /* Bytes not accepted: 0x80 - 0xBF are continuation bytes.
   * 0xC0 - 0xC1 can only start overlong forms, and 0xF5 - 0xFF
   * can only start values above U+10FFFF */
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

int utf8_code_validation(const unsigned char *buf, size_t len) {
  if (len == 0 || len > 4 || utf8_lead_len(buf[0]) != len)
    return 0;

  /* Each continuation byte must be 10xxxxxx. */
  for (size_t i = 1; i < len; i++) {
    if ((buf[i] & 0xC0) != 0x80)
      return 0;
  }

  /* If the sequence is 1 byte return success immediately. */
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

/* decode a complete, valid UTF-8 sequence of len bytes (1-4).
 * The caller is responsible for validation */
int utf8_decode(const unsigned char *buf, size_t len, uint32_t *cp) {
  if (!utf8_code_validation(buf, len))
    return -1;

  /* Start with the payload bits of the lead byte. Then add 6 bits
   * from each continuation byte. */
  uint32_t value = buf[0] & lead_mask[len];

  for (size_t i = 1; i < len; i++) {
    value = (value << 6) | (buf[i] & 0x3F);
  }

  *cp = value;
  return 0;
}
