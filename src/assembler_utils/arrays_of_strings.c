#include <stdio.h>
#include <stdlib.h>
#include "arrays_of_strings.h"


arrays_of_strings_t *initialise(void){
    arrays_of_strings_t *s_arrays = malloc(sizeof(arrays_of_strings_t));
    if (!s_arrays) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    s_arrays->capacity = INITIAL_ARRAY_SIZE;
    s_arrays->arrays = malloc(s_arrays->capacity * sizeof(array_of_strings_t *));
    if (!s_arrays->arrays) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    s_arrays->size = 0;
    return s_arrays;
}

void add(arrays_of_strings_t *s_arrays, array_of_strings_t *array){
    if (s_arrays->capacity<=s_arrays->size) {
        s_arrays->capacity *= 2;
        s_arrays->arrays = realloc(s_arrays->arrays,
                                        s_arrays->capacity
                                        * sizeof(array_of_strings_t *));
        if (!s_arrays->arrays) {
            perror("Cannot expand the size");
            exit(EXIT_FAILURE);
        }
    }
    s_arrays->arrays[s_arrays->size] = array;
    s_arrays->size++;
}

void free(arrays_of_strings_t *s_arrays) {
    for (int i = 0; i < s_arrays->size; i++) {
        for (int j = 1; j < s_arrays->arrays[i]->size; j++) {
            free(s_arrays->arrays[i]->array[j]);
        }
        free(s_arrays->arrays[i]->array);
        free(s_arrays->arrays[i]);
    }
    free(s_arrays->arrays);
    free(s_arrays);
}

