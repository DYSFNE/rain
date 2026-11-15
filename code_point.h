/*
**  2026 November 15
**
**  Copyright (C) 2026 DYSFNE <DYSFNE@gmail.com>
**  SPDX-License-Identifier: 0BSD
**
*************************************************************************
**  This header file defines the interface to the code point printer.  It
**  reads UTF-8 input and prints the Unicode code point of each character
**  on its own line.
*/
#ifndef RAIN_CODE_POINT_H
#define RAIN_CODE_POINT_H

#include <stdio.h>

/*
**  Read UTF-8 bytes from in until end of file.  For each valid character,
**  write one line to out with its byte offset and its code point.  For
**  each invalid byte, report the byte and its offset.
**
**  Return 0 on success.  Return -1 if a read error occurs on in or a write
**  error occurs on out.  Invalid input is not an error: the function
**  reports it and continues.
*/
extern int code_point(FILE *in, FILE *out);

#endif /* RAIN_CODE_POINT_H */
