//
// Created by cristina on 09/06/2020.
//

#include "parser.h"
#include "define_types.h

/*
 * returns the corresponding enum of a given string_opcode
 */
mnemonic_t string_to_mnemonic(const char *string) {
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

    exit(EXIT_FAILURE);
}


void parse_branch(Token *token, const char *label);

Address parse_address(const char *address, Token *token);

/*
 * parses an operand2 which is either a shifted register or #expression
 */
Operand2 parse_operand2(const char *operand) {
    Operand2 operand2;
    if (operand[0] == '#') {
        operand2.immediate = 1;
        operand2.expression = parse_expression(operand + 1);
    } else {
        operand2.immediate = 0;
        operand2.shifted_register.rm = atoi(operand + 1);

        char *shft = split((char **) &operand, ' ', 1);
        if (strcmp(shft, "")) {
            operand2.shifted_register.shift = parse_shift(skip_whitespace(shft));
        }
    }
    return operand2;
}

/*
 * parses a data processing instruction
 */
void parse_data_processing(Token *token, const char *string) {

    char *copy_string = calloc(strlen(string) + 1, sizeof(char));
    strcpy(copy_string, string);
    char *operand2;

    switch (token->opcode) {
        case MOV:
            operand2 = split(copy_string, ',', 1);
            token->data_pocessing.rd = atoi(copy_string + 1);
            break;
        case TST:
        case TEQ:
        case CMP:
        case LSL:
            operand2 = split(&copy_string, ',', 1);
            token->data_processing.rn = atoi(copy_string + 1);
            break;
        case ANDEQ:
            token->condition = EQ;
        default:
            operand2 = split(&copy_string, ',', 2);
            token->data_processing.rd = atoi(copy_string + 1);
            token->data_processing.rn = atoi(copy_string + 4);
    }
    token->data_processing.op2 = parse_operand2(skip_whitespace(operand2));
    free(copy_string);
}

void parse_transfer(Token *token, const char *transfer);

/*
 * parses special instructions (andeq, lsl)
 */
void parse_special(Token *token, const char *string) {
    char *copy_string = malloc(strlen(string) + 1);
    strcpy(copy_string, string);
    char *operand2;

    operand2 = split(&copy_string, ',', 1);
    token->data_processing.rd = atoi(copy_string) + 1;
    token->data_processing.op2 = parse_operand2(operand2);
    free(copy_string);
}

/*
 * parses an expression
 */
static int parse_expression(const char *expression) {
    if (!strncmp(expression, "0x", 2)) { return strtol(expression + 2, NULL, 16); }
        //HEX
    else {return strtol(expression, NULL, 10);}  //DECIMAL
    if(!strncmp(expression,"-",1)){
        if(!strncmp(expression+1,"0x",2)){
            return (-1)*strtol(expression+3, NULL, 16);
        }
    }
    return (-1)*strtol(expression+1),NULL, 10);
}

/*
 * returns the corresponding enum of a given string_shift
 */
shift_t string_to_shift(const char *string){
    if (!strcmp(str, "lsl")) {
        return SHIFT_LSL;
    }
    if (!strcmp(str, "lsr")) {
        return SHIFT_LSR;
    }
    if (!strcmp(str, "asr")) {
        return SHIFT_ASR;
    }
    if (!strcmp(str, "ror")) {
        return SHIFT_ROR;
    }

    exit(EXIT_FAILURE);
}
/*
 * parses a shift
 */
Shift parse_shift(const char *shift){
Shift shift1;
shift1.type=string_to_shift(shift);

char *copy_shift = calloc(strlen(shift)+1, sizeof(char ));
strcpy(copy_shift, shift);
char *copy_shift_split

}

void parse_general(Token *token, char *instruction, int address);

char *skip_whitespace(const char *string);

char *split(char **pointers, char separator, unsigned int number_occurrence);

char **split_until(char **pointers, char *separator, unsigned int number_occurrences);
