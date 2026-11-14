/*
**  November 14, 2026
**
**  Copyright (C) 2026 DYSFNE <DYSFNE@gmail.com>
**  SPDX-License-Identifier: 0BSD
**
**************************************************************************
**  This header file defines a macro to get the number of elements in an
**  array.
**
*/
#ifndef _RAIN_ARRAY_COUNT_H_
#define _RAIN_ARRAY_COUNT_H_

/*
**  Return the number of elements in an array.
**
**  The parameter arr must be an array object, not a pointer.  If the
**  caller passes a pointer to arr the result is wrong and the compiler
**  will not give an error.
**
**  TODO: Make the compiler reject a pointer as arr.
*/
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif /* _RAIN_ARRAY_COUNT_H_ */
