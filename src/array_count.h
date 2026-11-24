/*
 *  @file:          array_count.h
 *  @description:   Defines a macro to get the number of elements
 *                  in an array.
 */

#ifndef RAIN_ARRAY_COUNT_H
#define RAIN_ARRAY_COUNT_H

/*
 *  @macro:         ARRAY_COUNT(arr)
 *  @description:   Get the number of elements in an array.\
 *
 *  @arr:           The array whose size you want.
 *
 *  @return:        The size of the array.
 *
 *  @note:          The parameter @arr must be an array object,
 *                  not a pointer.
 *
 *  TODO: Make the compiler reject a pointer passed to @arr
 */
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif /* RAIN_ARRAY_COUNT_H */
