//
// Created by cristina on 09/06/2020.
//
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "define_types.h"

static mnemonic_t string_to_mnemonic(const char *string);

static shift_t string_to_shift(const char *string);

static void parse_branch(Token *token, const char *label);

static Address parse_address(const char *string_address, Token *token);

static Operand2 parse_operand2(char *operand);

static Shift parse_shift(const char *shift);

static void parse_data_processing(Token *token, const char *arguments);

static void parse_multiply(Token *token, const char *string_multiply);

static void parse_transfer(Token *token, const char *string_transfer);

static void parse_special(Token *token, const char *string);

static int parse_expression(const char *expression);

static char *skip_whitespace(const char *string);

static char *split(char *pointers, char separator, unsigned int number_occurrence);

static char **split_and_store(char *pointers, char *separator, unsigned int number_occurrences);

/*
 * returns the corresponding enum of a given string_opcode
 */
static mnemonic_t string_to_mnemonic(const char *string) {
    if (!strcmp(string, "add")) {
        return ADD;
    }

    if (!strcmp(string, "sub")) {
        return SUB;
    }

    if (!strcmp(string, "rsb")) {
        return RSB;
    }

    if (!strcmp(string, "and")) {
        return AND;
    }

    if (!strcmp(string, "eor")) {
        return EOR;
    }

    if (!strcmp(string, "orr")) {
        return ORR;
    }

    if (!strcmp(string, "mov")) {
        return MOV;
    }

    if (!strcmp(string, "tst")) {
        return TST;
    }

    if (!strcmp(string, "teq")) {
        return TEQ;
    }

    if (!strcmp(string, "cmp")) {
        return CMP;
    }

    if (!strcmp(string, "mul")) {
        return MUL;
    }

    if (!strcmp(string, "mla")) {
        return MLA;
    }

    if (!strcmp(string, "ldr")) {
        return LDR;
    }

    if (!strcmp(string, "str")) {
        return STR;
    }

    if (!strcmp(string, "b")) {
        return B;
    }

    if (!strcmp(string, "beq")) {
        return BEQ;
    }

    if (!strcmp(string, "bge")) {
        return BGE;
    }

    if (!strcmp(string, "blt")) {
        return BLT;
    }

    if (!strcmp(string, "bge")) {
        return BGE;
    }

    if (!strcmp(string, "bgt")) {
        return BGT;
    }

    if (!strcmp(string, "ble")) {
        return BLE;
    }

    if (!strcmp(string, "lsl")) {
        return LSL;
    }

    if (!strcmp(string, "andeq")) {
        return ANDEQ;
    }
    printf("there is no corresponding enum for given string");
    exit(EXIT_FAILURE);
}


static void parse_branch(Token *token, const char *label) {
    assert(token != NULL);
    assert(label != NULL);

    switch (token->opcode) {
        case B:
            token->condition = AL;
            break;
        case BEQ:
            token->condition = EQ;
            break;
        case BNE:
            token->condition = NE;
            break;
        case BGE:
            token->condition = GE;
            break;
        case BLT:
            token->condition = LT;
            break;
        case BGT:
            token->condition = GT;
            break;
        case BLE:
            token->condition = LE;
            break;
        default:
            printf("Unexpected opcode");
            exit(EXIT_FAILURE);
    }

    token->Content.branch.expression = (char *) label;
}

static Address parse_address(const char *string_address, Token *token) {

    Address address;
    if (string_address[0] == '=') { //expression
        address.format = 0;
        token->Content.transfer.address.Expression.expression = parse_expression(string_address + 1);
        address.Expression.expression = parse_expression(string_address + 1);

        if (token->Content.transfer.address.Expression.expression >= 256) {
            token->flag = 1;
        }

        return address;
    }

    address.format = 1;
    address.Expression.Register.rn = atoi(string_address + 2);

    if (string_address[strlen(string_address) - 1] == ']') {
        address.Expression.Register.pre_post_index = 0;
    } else {
        address.Expression.Register.pre_post_index = 1;
    }

    char *copy_address = calloc(strlen(string_address) + 1, sizeof(char));
    strcpy(copy_address, string_address + 1);

    char *rm_expression = split(copy_address, ',', 1);
    rm_expression = skip_whitespace(rm_expression);

    if (!strcmp(rm_expression, "")) {
        address.Expression.expression = 0;
        address.Expression.Register.format = 0;

    } else {

        if (rm_expression[0] == '#') {
            address.Expression.Register.format = 0;
            address.Expression.expression = parse_expression(skip_whitespace(rm_expression) + 1);
        } else { //shifted register
            address.Expression.Register.format = 1;


            if (copy_address[0] == '+' || copy_address[0] == '-') {
                address.Expression.Register.Offset.Shift.pm = (string_address[0] == '+' ? 0 : 1);
                copy_address++;
            }
            address.Expression.Register.Offset.Shift.pm = 0;

            address.Expression.Register.Offset.Shift.rm = atoi(skip_whitespace(rm_expression) + 1);

            char *new_shift = split(rm_expression, ',', 1);

            if (!(strcmp(new_shift, ""))) {
                Shift no_shift;
                no_shift.type = NO_SHIFT;
                address.Expression.Register.Offset.Shift.shift = no_shift;
            } else {
                address.Expression.Register.Offset.Shift.shift = parse_shift(skip_whitespace(new_shift));
            }
        }
    }

    free(copy_address);
    return address;
}

/*
 * parses an operand2 which is either a shifted register or #expression
 */
static Operand2 parse_operand2(char *operand) {
    Operand2 operand2;
    if (operand[0] == '#') {
        operand2.immediate = 1;
        operand2.Register.expression = parse_expression(operand + 1);
    } else {
        operand2.immediate = 0;
        operand2.Register.shifted_register.rm = atoi(operand + 1);
        char *shft;
        shft = strtok(operand, ",");
        if (shft != NULL) {
            operand2.Register.shifted_register.shift = parse_shift(skip_whitespace(shft));
        } else {
            operand2.Register.shifted_register.shift.type = NO_SHIFT;
        }
    }
    return operand2;
}

/*
 * parses a data processing instruction
 */
static void parse_data_processing(Token *token, const char *string) {

    char *copy_string = calloc(strlen(string) + 1, sizeof(char));
    strcpy(copy_string, string);
    char *operand2;

    switch (token->opcode) {
        case MOV:
            operand2 = split(copy_string, ',', 1);
            token->Content.data_processing.rd = atoi(copy_string + 1);
            break;
        case TST:
        case TEQ:
        case CMP:
        case LSL:
            operand2 = split(copy_string, ',', 1);
            token->Content.data_processing.rn = atoi(copy_string + 1);
            break;
        case ANDEQ:
            token->condition = EQ;
        default:
            operand2 = split(copy_string, ',', 2);
            token->Content.data_processing.rd = atoi(copy_string + 1);
            token->Content.data_processing.rn = atoi(copy_string + 4);
    }
    token->Content.data_processing.op2 = parse_operand2(skip_whitespace(operand2));
    free(copy_string);
}

static void parse_transfer(Token *token, const char *string_transfer) {
    token->Content.transfer.rd = atoi(string_transfer);

    char *copy_transfer = calloc(strlen(string_transfer) + 1, sizeof(char));
    strcpy(copy_transfer, string_transfer);
    char *copy_transfer_split = split(copy_transfer, ',', 1);
    token->Content.transfer.address = parse_address(skip_whitespace(copy_transfer_split), token);
    free(copy_transfer_split);
}

/*
 * parses special instructions (andeq, lsl)
 */
static void parse_special(Token *token, const char *string) {
    char *copy_string = malloc(strlen(string) + 1);
    strcpy(copy_string, string);
    char *operand2;

    operand2 = split(copy_string, ',', 1);
    token->Content.data_processing.rd = atoi(copy_string) + 1;
    token->Content.data_processing.op2 = parse_operand2(operand2);
    free(copy_string);
}

/*
 * parses an expression
 */
static int parse_expression(const char *expression) {

    if (!strncmp(expression, "-", 1)) {
        if (!strncmp(expression + 1, "0x", 2)) {
            return -strtol(expression + 3, NULL, 16);
        } else {
            return -strtol(expression + 1, NULL, 10);
        }
    }
    if (!strncmp(expression, "0x", 2)) {  //HEX
        return strtol(expression + 2, NULL, 16);
    } else { return strtol(expression, NULL, 10); }  //DECIMAL
}


/*
 * returns the corresponding enum of a given string_shift
 */
static shift_t string_to_shift(const char *string) {
    if (!strcmp(string, "lsl")) {
        return SHIFT_LSL;
    }
    if (!strcmp(string, "lsr")) {
        return SHIFT_LSR;
    }
    if (!strcmp(string, "asr")) {
        return SHIFT_ASR;
    }
    if (!strcmp(string, "ror")) {
        return SHIFT_ROR;
    }

    exit(EXIT_FAILURE);
}

/*
 * parses a shift
 */
static Shift parse_shift(const char *shift) {
    Shift shift1;
    shift1.type = string_to_shift(shift);

    char *copy_shift = calloc(strlen(shift) + 1, sizeof(char));
    strcpy(copy_shift, shift);
    char *copy_shift_split = split(copy_shift, ' ', 1);
    if (copy_shift_split[0] == 'r') {
        shift1.format = 1;
        shift1.args.expression = parse_expression(copy_shift_split);
    }

    free(copy_shift);
    return shift1;
}

static void parse_multiply(Token *token, const char *string_multiply) {
    char *copy_multiply = calloc(strlen(string_multiply) + 1, sizeof(char));
    strcpy(copy_multiply, string_multiply);

    char **tokens = split_and_store(copy_multiply, ",", 3);
    token->Content.multiply.rd = atoi(tokens[0] + 1);
    token->Content.multiply.rm = atoi(tokens[1] + 1);
    token->Content.multiply.rs = atoi(tokens[2] + 1);

    if (token->opcode == MLA) {
        token->Content.multiply.rn = atoi(tokens[3] + 1);
    } else {
        token->Content.multiply.rn = 0;
    }
    free(copy_multiply);
}


void parse_general(Token *token, char *instruction) {
    assert(token != NULL);
    assert(instruction != NULL);

    //arguments after opcode
    char *args = strtok(instruction, " ");
    printf("%s\n", instruction);
    mnemonic_t operation = string_to_mnemonic(instruction);

    token->opcode = operation;
    token->flag = 0;
    token->condition = AL;
    args = skip_whitespace(args);

    if (operation == LSL) {
        parse_special(token, args);

    } else if (operation <= CMP) {
        parse_data_processing(token, args);

    } else if (operation <= MLA) {
        parse_multiply(token, args);

    } else if (operation <= STR) {
        parse_transfer(token, args);

    } else if (operation <= B) {
        parse_branch(token, args);

    } else {
        printf("Error in parse_general.\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * returns a pointer to the address of the first non-whitespace character in a string
 */
static char *skip_whitespace(const char *string) {
    char *copy_string = (char *) string;
    while (isspace(*copy_string)) {
        strcpy(copy_string, copy_string + 1);
    }
    return copy_string;
}

/*
 * splits a string by a given separator, replacing it with '\0', like strtok,
 * but only for a number of  occurences
 */
static char *split(char *pointers, char separator, unsigned int number_occurrence) {
    int counter = 0, i = 0;
    size_t length = strlen(pointers);
    while (i < length) {
        if ((pointers)[i] == separator) {
            counter++;
            if (counter == number_occurrence)break;
        }
        i++;
    }
    return (pointers) + i + 1;
}

/*
 * splits and stores the pointers in an array
 */
static char **split_and_store(char *pointers, char *separator, unsigned int number_occurrences) {
    char **tokens = malloc(sizeof(char *) * (number_occurrences + 1));

    if (tokens == NULL) {
        printf("buffer allocation failed in split_and_store");
        exit(EXIT_FAILURE);
    }

    unsigned int number_tokens = 0;
    while ((tokens[number_tokens] = strtok(pointers, separator)) != NULL &&
           number_tokens != number_occurrences) {
        number_tokens++;
    }
    if (0) {
        if (number_occurrences != number_tokens) {
            printf("the function didn't split enough");
        }
    }
    return tokens;
}
