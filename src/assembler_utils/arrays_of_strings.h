//
// Created by cristina on 01/06/2020.
//

#ifndef ARM11_18_ARRAYS_OF_STRINGS_H
#define ARM11_18_ARRAYS_OF_STRINGS_H

#include "array_of_strings.h"
#include "constants.h"

/*
 * @brief we create a struct in order to hold an array of string arrays and its size
 */
typedef struct {
    uint16_t capacity; //maximum number of string arrays
    array_of_strings_t **arrays;
    uint16_t size;
} arrays_of_strings_t;

//initialises an arrays_of_strings_t
arrays_of_strings_t *initialise(void);

//adds an array to an arrays_of strings_t
void add(arrays_of_strings_t *s_arrays, array_of_strings_t *array);

//frees the allocated memory in an arrays_of_strings_t
void free(arrays_of_strings_t *s_arrays);

#endif //ARM11_18_ARRAYS_OF_STRINGS_H
