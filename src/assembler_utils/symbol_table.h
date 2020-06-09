#ifndef ARM11_18_SYMBOL_TABLE_H
#define ARM11_18_SYMBOL_TABLE_H

#include "arrays_of_strings.h"
#include "constants.h"
#include "define_types.h"

/**
 * @brief Struct that represents a row from a symbol table
 * it is made out of a string label and its address
 */
typedef struct {
    char label[MAX_LABEL_LENGTH + 1];
    address_t address;
} symbol_table_row_t;

/**
 * @brief Struct that represents a symbol table.
 * it is made out of a capacity (maximum number of rows),
 * a size(the number of rows) and an array of rows.
 */
typedef struct {
    uint16_t capacity;
    uint16_t size;
    symbol_table_row_t *rows;
} symbol_table_t;

symbol_table_t *generate_table(arrays_of_strings_t *tokens);

address_t get_address(symbol_table_t *table, char *label);

void free_table(symbol_table_t *table);

#endif //ARM11_18_SYMBOL_TABLE_H
