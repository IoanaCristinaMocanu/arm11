#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "define_types.h"

/*
 * @brief creates and initialises a symbol table
 * @param initial_size is the initial maximum number of rows
 */
static symbol_table_t *create_table(uint16_t initial_size) {
    symbol_table_t *table = malloc(sizeof(symbol_table_t));
    if (!table) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    table->rows = malloc(sizeof(symbol_table_row_t) * initial_size);
    if (!table->rows) {
        perror("Unable to allocate memory for symbol table");
        exit(EXIT_FAILURE);
    }
    table->capacity = initial_sizer;
    table->size = 0;
    return table;}


static void add_row(symbol_table_t *table, char *label, address_t address) {
    if (is_label_in_table(table, label)) {
        fprintf(stderr, "Label already in symbol table");
        exit(EXIT_FAILURE);
    }
    symbol_table_row_t row = {
            .address = address,
    };
    memcpy(row.label, label, MAX_LABEL_LENGTH + 1);
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->rows = realloc(table->rows, table->capacity
                                           * sizeof(symbol_table_row_t));
        if (!table->rows) {
            perror("Unable to expand size of symbol table");
            exit(EXIT_FAILURE);
        }
    }
    table->rows[table->size] = row;
    ++table->size;
}


symbol_table_t *generate_table(arrays_of_strings_t *tokens){
    symbol_table_t *table = create_table(16);
    address_t address = 0;

    for (int i = 0; i < tokens->size; i++) {
        if (tokens->arrays[i]->size != 1) {
            address += 4;
        } else {
            for (int j = 0; tokens->arrays[i]->array[0][j]; j++) {
                if (tokens->arrays[i]->array[0][j] == ':') {
                    tokens->arrays[i]->array[0][j] = '\0';
                }
            }
            add_row(table, tokens->arrays[i]->array[0], address);
            tokens->arrays[i]->array[0] = '\0';
        }
    }

    return table;
}


