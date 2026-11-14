/**
 * @file array_count.h
 * @brief Macro to get the number of elements in an array.
 */

#ifndef RAIN_ARRAY_COUNT_H
#define RAIN_ARRAY_COUNT_H

/**
 * @brief Get the number of elements in an array.
 *
 * @param arr  An array object (not a pointer).
 *
 * @return Number of elements in @p arr, as a size_t constant.
 *
 * @warning Use only with a real array. If @p arr is a pointer,
 *          the result is wrong and the compiler gives no error.
 *          A function parameter declared as an array is a pointer.
 */
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif /* RAIN_ARRAY_COUNT_H */
