//
// Created by cristina on 09/06/2020.
//

#ifndef ARM11_18_PARSER_H
#define ARM11_18_PARSER_H

#include "define_types.h"
#include <stdio.h>

mnemonic_t string_to_mnemonic(const char *string);

shift_t string_to_shift(const char *string);

void parse_branch(Token *token, const char *label);

Address parse_address(const char *string_address, Token *token);

Operand2 parse_operand2(const char *operand);

Shift parse_shift(const char *shift);

void parse_data_processing(Token *token, const char *arguments);

void parse_multiply(Token *token, const char *string_multiply);

void parse_transfer(Token *token, const char *string_transfer);

void parse_special(Token *token, const char *string);

static int parse_expression(const char *expression);


void parse_general(Token *token, char *instruction);

char *skip_whitespace(const char *string);

char *split(char **pointers, char separator, unsigned int number_occurrence);

char **split_and_store(char **pointers, char *separator, unsigned int number_occurrences);

#endif //ARM11_18_PARSER_H
