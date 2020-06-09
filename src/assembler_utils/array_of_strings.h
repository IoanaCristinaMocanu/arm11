//
// Created by cristina on 30/05/2020.
//

#ifndef ARM11_18_ARRAY_OF_STRINGS_H
#define ARM11_18_ARRAY_OF_STRINGS_H

#include <stdint.h>

/*
 * @brief we create a struct in order to hold an array of strings and its size
 */
typedef struct {
    char **array;
    uint16_t size;
} array_of_strings_t;

#endif //ARM11_18_ARRAY_OF_STRINGS_H
