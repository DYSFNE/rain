/*
 *  @file:          codepoint.h
 *  @description:   Public interface for printing Unicode code points and
 *                  reporting invalid UTF-8 input bytes.
 */


#ifndef RAIN_CODEPOINT_H
#define RAIN_CODEPOINT_H


#include <stddef.h>
#include <stdint.h>
#include <stdio.h>


/*
 * Buffer capacities and UTF-8 processing limits, in bytes.
 *
 * The output buffer must accommodate one complete formatted line.
 * The internal window must accommodate one complete UTF-8 sequence.
 */
enum {
  CP_IN_BUF_SIZE = 128,
  CP_OUT_BUF_SIZE = 128,
  CP_WIN_BUF_SIZE = 128,
  CP_MAX_LINE = 40, /* includes the null terminator */
  CP_UTF8_MAX = 4,
};


/* Indicates whether additional input can be supplied. */
typedef enum {
  RAIN_EOF_OFF,
  RAIN_EOF_ON
} rain_eof;


/* Processing statuses returned by the decoder. */
typedef enum {
  CP_ERROR,
  CP_NEED_INPUT,
  CP_NEED_OUTPUT,
  CP_PROCEED,
  CP_DONE
} cp_process_status;


/* Types of results produced by the code point decoder. */
typedef enum {
  CP_VALID,
  CP_INVALID
} cp_result_type;


/* Persistent state of the code point decoder. */
typedef struct cp_state_s {
  unsigned char window[CP_WIN_BUF_SIZE]; /* internal decoder buffer */
  size_t pos_window;                     /* next unprocessed byte */
  size_t avail_window;                   /* unprocessed window bytes */

  size_t avail_in;                       /* unconsumed input bytes */
  size_t avail_out;                      /* available output capacity */

  unsigned char *pos_in;                 /* next unread input byte */
  char *pos_out;                         /* next writable output byte */

  uint64_t offset;                       /* input stream byte offset */
} cp_state;


/*
 * Result of processing one valid code point or invalid input byte.
 *
 * The type member determines which union member is active.
 */
typedef struct {
  cp_result_type type;  /* selects the active union member */
  uint64_t offset;      /* byte offset in the input stream */
  size_t length;        /* number of input bytes represented */

  union {
    uint32_t codepoint; /* decoded Unicode code point */
    unsigned char byte; /* invalid input byte */
  } value;
} cp_result;


/*
 *  @function:      codepoint()
 *  @description:   Reads input as UTF-8 and prints Unicode code points
 *                  with their byte offsets. Invalid bytes are reported
 *                  without terminating processing.
 *
 *  @src:           Readable input stream.
 *  @dest:          Writable output stream.
 *
 *  @return:        0 on success, or -1 on a read, write, or processing
 *                  error.
 */
int codepoint(FILE *src, FILE *dest);

#endif /* RAIN_CODEPOINT_H */
