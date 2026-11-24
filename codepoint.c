/*
 *  @file:         codepoint.c
 *  @description:  Implements a streaming UTF-8 code point printer.
 *                 Manages input and output buffers, decodes UTF-8
 *                 sequences, and formats code points with their byte
 *                 offsets. Invalid bytes are reported individually.
 */

#include "codepoint.h"
#include "utf8.h"

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

/* INCLUDES FROM THE HEADER */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>


/*
 *  @function:      cp_read_buf()
 *  @implementation:
 *                  Compacts unprocessed window bytes to the beginning
 *                  of the buffer before appending available input.
 *                  Preserves incomplete UTF-8 sequences across refills.
 */
static void cp_read_buf(cp_state *state) {
  size_t compact_end; /* end offset of compacted data*/
  size_t win_space;   /* available capacity after compaction */
  size_t copy_len;    /* number of input bytes to copy */

  /* Move unprocessed bytes to the beginning of the window. */
  memmove(state->window, state->window + state->pos_window,
          state->avail_window);

  state->pos_window = 0;

  /* Calculate the available space after the retained bytes. */
  compact_end = state->avail_window;
  win_space = sizeof(state->window) - compact_end;

  /* Limit the copy to the available input and window capacity. */
  copy_len = (state->avail_in > win_space) ? win_space : state->avail_in;

  /* Avoid passing a potentially null input pointer to memcpy. */
  if (copy_len > 0) {
    memcpy(state->window + compact_end, state->pos_in, copy_len);

    /* Update the input and window positions after copying. */
    state->avail_in -= copy_len;
    state->pos_in += copy_len;
    state->avail_window += copy_len;
  }
}


/*
 *  @function:      cp_commit()
 *  @implementation:
 *                  Advances input and output positions after successful
 *                  formatting. Input consumption and output production
 *                  are committed together.
 */
static void cp_commit(cp_state *state, size_t consumed, int produced) {
  /* Commit input consumption. */
  state->pos_window += consumed;
  state->avail_window -= consumed;
  state->offset += consumed;

  /* Commit output production. */
  state->pos_out += produced;
  state->avail_out -= produced;
}


/*
 *  @function:      cp_make_valid()
 *  @implementation:
 *                  Initializes a result with type CP_VALID, the input
 *                  byte offset, decoded code point, and sequence length.
 */
static cp_result cp_make_valid(uint64_t offset, uint32_t cp, size_t length) {
  cp_result result = {.type = CP_VALID,
                      .offset = offset,
                      .length = length,
                      .value.codepoint = cp};

  return result;
}


/*
 *  @function:      cp_make_invalid()
 *  @implementation:
 *                  Initializes a result with type CP_INVALID, the input
 *                  byte offset, invalid byte, and a length of one.
 */
static cp_result cp_make_invalid(uint64_t offset, unsigned char byte) {
  cp_result result = {
      .type = CP_INVALID, .offset = offset, .length = 1, .value.byte = byte};

  return result;
}


/*
 *  @function:      cp_format()
 *  @implementation:
 *                  Uses snprintf() to produce a formatted line based
 *                  on the result type. The returned length excludes
 *                  the terminating null byte.
 */
static int cp_format(char *dest, size_t size, const cp_result *result) {
  int n;

  /* TODO: Pass control of the hexadecimal offset size to the user. */
  switch (result->type) {
  case CP_VALID:
    /* Format an output line for a valid UTF-8 code point sequence. */
    n = snprintf(dest, size, "0x%016" PRIX64 "  U+%04" PRIX32 "\n",
                 result->offset, result->value.codepoint);
    break;
  case CP_INVALID:
    /* Format an output line for an invalid byte. */
    n = snprintf(dest, size, "0x%016" PRIX64 "  Invalid byte 0x%02X\n",
                 result->offset, (unsigned int)result->value.byte);
    break;
  }

  return n;
}


/*
 *  @function:      cp_decoder_state()
 *  @implementation:
 *                  Uses four-byte lookahead to determine whether
 *                  processing can continue. At EOF, permits processing
 *                  with fewer bytes to handle incomplete sequences.
 */
static cp_process_status cp_decoder_state(cp_state *state, rain_eof final) {
  /* Enough buffered input and output space to process the next item. */
  if (state->avail_window >= CP_UTF8_MAX && state->avail_out >= CP_MAX_LINE)
    return CP_PROCEED;

  /* No unprocessed input remains and EOF has been reached. */
  if (state->avail_window == 0 && state->avail_in == 0 && final == RAIN_EOF_ON)
    return CP_DONE;

  /* Insufficient space for a complete formatted output line. */
  if (state->avail_out < CP_MAX_LINE)
    return CP_NEED_OUTPUT;

  /* Refill the internal window with available input. */
  if (state->avail_in > 0)
    cp_read_buf(state);

  if (state->avail_window >= CP_UTF8_MAX)
    return CP_PROCEED;

  /* Request more input if the window lacks sufficient lookahead. */
  if (final == RAIN_EOF_OFF)
    return CP_NEED_INPUT;

  /* Process remaining bytes at EOF, including incomplete sequences. */
  return CP_PROCEED;
}


/*
 *  @function:      cp_process()
 *  @implementation:
 *                  Repeatedly processes buffered input until additional
 *                  resources are required or decoding finishes. Invalid
 *                  UTF-8 sequences are reported one byte at a time.
 */
static cp_process_status cp_process(cp_state *state, rain_eof final) {
  for (;;) {
    /* Ensure the buffers are in a state that can process the next byte. */
    cp_process_status curr_state = cp_decoder_state(state, final);

    switch (curr_state) {
    case CP_NEED_INPUT:
      return CP_NEED_INPUT;
    case CP_NEED_OUTPUT:
      return CP_NEED_OUTPUT;
    case CP_DONE:
      return CP_DONE;
    case CP_PROCEED: {
      int written;
      /* Consume one byte when the UTF-8 sequence is invalid. */
      size_t consumed = 1;
      size_t length = utf8_lead_len(state->window[state->pos_window]);
      bool valid = true;
      cp_result result;

      /* Reject invalid, incomplete, or malformed UTF-8 sequences. */
      if (length == 0 || state->avail_window < length ||
          (utf8_is_valid_seq(state->window + state->pos_window, length)) == 0)
        valid = false;

      if (valid) {
        uint32_t cp;

        if (utf8_decode(state->window + state->pos_window, length, &cp) < 0)
          return CP_ERROR;

        result = cp_make_valid(state->offset, cp, length);
        consumed = length;
      } else {
        /* Consume only the first byte of an invalid sequence. */
        result =
            cp_make_invalid(state->offset, state->window[state->pos_window]);
      }

      written = cp_format(state->pos_out, state->avail_out, &result);

      if (written < 0 || (size_t)written >= state->avail_out)
        return CP_ERROR;

      cp_commit(state, consumed, written);
      break;
    }
    default:
      return CP_ERROR;
    }
  }
}


/*
 *  @function:      codepoint()
 *  @implementation:
 *                  Uses an outer loop to replenish input and an inner
 *                  loop to process buffered data. Writes formatted
 *                  output when requested or when decoding completes.
 */
int codepoint(FILE *src, FILE *dest) {
  unsigned char in[CP_IN_BUF_SIZE];
  char out[CP_OUT_BUF_SIZE];
  rain_eof eof = RAIN_EOF_OFF;
  cp_state state = {0};

  state.pos_out = out;
  state.avail_out = sizeof(out);

  /* Supply new input only when the decoder request more data. */
  for (;;) {
    state.avail_in = fread(in, 1, sizeof(in), src);

    if (ferror(src))
      return -1;

    eof = feof(src) ? RAIN_EOF_ON : RAIN_EOF_OFF;

    state.pos_in = in;

    /* Process input until more data is needed or processing ends. */
    for (;;) {
      int status = cp_process(&state, eof);

      if (status == CP_NEED_INPUT) {
        break;
      } else if (status == CP_NEED_OUTPUT) {
        /* Drain the output buffer before resuming processing. */
        size_t to_write = state.pos_out - out;

        if (fwrite(out, 1, to_write, dest) < to_write)
          return -1;

        state.pos_out = out;
        state.avail_out = sizeof(out);
      } else if (status == CP_ERROR) {
        return -1;
      } else if (status == CP_DONE) {
        /* Write any remaining formatted output before returning. */
        size_t to_write = state.pos_out - out;

        if (fwrite(out, 1, to_write, dest) < to_write)
          return -1;

        return 0;
      } else {
        return -1;
      }
    }
  }
}
