/**
 * @file utf8.h
 * @brief Classify, validate and decode UTF-8 sequences (RFC 3629).
 */

#ifndef RAIN_UTF8_H
#define RAIN_UTF8_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Get the sequence length from a lead byte
 *
 * @param[in] lead  First byte of a possible UTF-8 sequence
 *
 * @return Length of the sequence (1-4), or 0 if @p lead cannot
 *  start a valid sequence
 */
size_t utf8_lead_len(unsigned char lead);

/**
 * @brief Check that a buffer holds a valid UTF-8 sequence.
 *
 * Rejects overlong forms, surrogates (U+D800-U+DFFF) and values
 * above U+10FFFF.
 *
 * @param[in] buf   Bytes to check.
 * @param[in] len   Number of bytes in the sequence.
 *
 * @pre @p buf is not NULL and holds at least @p len bytes.
 *
 * @retval 1    @p buf[0..len - 1] is one complete, valid sequence.
 * @retval 0    The sequence is not valid.
 */
int utf8_is_valid_seq(const unsigned char *buf, size_t len);

/**
 * @brief Decode one UTF-8 sequence to a code point.
 *
 * Validates the sequence with utf8_is_valid_seq() before decoding.
 *
 * @param[in] buf   Bytes of the sequence
 * @param[in] len   Number of bytes in the sequence
 * @param[out] cp   Receives the code point on success
 *
 * @pre @p buf is not NULL and holds at least @p len bytes.
 * @pre @p cp is not NULL.
 *
 * @retval 0 Success. @p *cp holds the code point.
 * @retval -1 The sequence is not valid. @p *cp is not changed.
 */
int utf8_decode(const unsigned char *buf, size_t len, uint32_t *cp);

#endif /* RAIN_UTF8_H */
