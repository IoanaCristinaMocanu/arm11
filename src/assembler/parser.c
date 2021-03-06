//
// Created by cristina on 09/06/2020.
//
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "define_types.h"

static characteristics_t string_to_characteristic(const char *string);

static mnemonic_t string_to_mnemonic_stack(const char *string, Token *token);

static mnemonic_t string_to_mnemonic(const char *string);

static shift_t string_to_shift(const char *string);

static void parse_branch(Token *token, const char *label);

static Address parse_address(char *string_address, Token *token);

static Operand2 parse_operand2(char *operand);

static Shift parse_shift(const char *shift);

static void parse_data_processing(Token *token, const char *arguments);

static void parse_multiply(Token *token, const char *string_multiply);

static void parse_block_data_transfer(Token *token, const char *string);

static void parse_transfer(Token *token, const char *string_transfer);

static void parse_special(Token *token, const char *string);

static int parse_expression(char *expression);

static char *skip_whitespace(const char *string);

static char *split(char *pointers, char separator, unsigned int number_occurrence);

static char **split_and_store(char *pointers, char *separator, unsigned int number_occurrences);

static characteristics_t string_to_characteristic(const char *string) {

	if (!strcmp(string, "fd")) {
		return FD;
	}


	if (!strcmp(string, "ed")) {
		return ED;
	}


	if (!strcmp(string, "fa")) {
		return FA;
	}


	if (!strcmp(string, "ea")) {
		return EA;
	}

	printf("there is no corresponding enum for given string");
	exit(EXIT_FAILURE);
}

static mnemonic_t string_to_mnemonic_stack(const char *string, Token *token) {
	if (!strncmp(string, "ldm", 3)) {
		char *string_characteristic = calloc(strlen(string) + 1, sizeof(char));
		strncpy(string_characteristic, string + 3, 2);
		token->Content.block_data_transfer.characteristic = string_to_characteristic(string_characteristic);
		return LDM;
	}

	if (!strncmp(string, "stm", 3)) {
		char *string_characteristic = calloc(strlen(string) + 1, sizeof(char));
		strncpy(string_characteristic, string + 3, 2);
		token->Content.block_data_transfer.characteristic = string_to_characteristic(string_characteristic);

		return STM;
	}

	printf("there is no corresponding enum for given string");
	exit(EXIT_FAILURE);
}
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
	if (!strcmp(string, "beq")) {
		return BEQ;
	}
	if (!strcmp(string, "bne")) {
		return BNE;
	}
	if (!strcmp(string, "b")) {
		return B;
	}


	if (!strcmp(string, "bge")) {
		return BGE;
	}

	if (!strcmp(string, "blt")) {
		return BLT;
	}

	if (!strcmp(string, "bgt")) {
		return BGT;
	}

	if (!strcmp(string, "ble")) {
		return BLE;
	}
	if (!strcmp(string, "b")) {
		return B;
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

static Address parse_address(char *string_address, Token *token) {

	Address address;
	if (string_address[0] == '=') {     //expression
		address.format = 0;
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
	strcpy(copy_address, string_address + 1);     //skip '['

	char *rm_expression = split(copy_address, ',', 1);
	rm_expression = skip_whitespace(rm_expression);
	if (rm_expression[strlen(rm_expression) - 1] == ']') {
		rm_expression[strlen(rm_expression) - 1] = '\0';
	}
	if (!strcmp(rm_expression, "")) {
		address.Expression.expression = 0;
		address.Expression.Register.format = 0;

	} else {
		if (rm_expression[0] == '#') {
			address.Expression.Register.format = 0;
			address.Expression.expression = parse_expression(skip_whitespace(rm_expression) + 1);
			address.Expression.expression = parse_expression(skip_whitespace(rm_expression) + 1);
		} else {         //shifted register
			address.Expression.Register.format = 1;


			if (copy_address[0] == '+' || copy_address[0] == '-') {
				address.Expression.Register.Offset.Shift.pm = (string_address[0] == '+' ? 0 : 1);
				copy_address++;
			}
			address.Expression.Register.Offset.Shift.pm = 0;

			address.Expression.Register.Offset.Shift.rm = (unsigned int) atoi(skip_whitespace(rm_expression) + 1);
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

static void parse_block_data_transfer(Token *token, const char *string) {
	assert(token != NULL);
	assert(string != NULL);

	char *copy_string = calloc(strlen(string) + 1, sizeof(char));
	strcpy(copy_string, string);

	char *copy_string1 = calloc(strlen(string) + 1, sizeof(char));
	strcpy(copy_string1, string);

	char *rn_string = strtok(copy_string, ",");

	int size = strlen(rn_string);

	if (rn_string[size - 1] == '!') {
		token->Content.block_data_transfer.write_back_bit = 1;
		rn_string[2] = '\0';

		if (!strcmp(rn_string, "sp")) { token->Content.block_data_transfer.rn = 13; }
		else {
			token->Content.block_data_transfer.rn = atoi(rn_string + 1);
		}
		copy_string1 += 4;

	} else {
		token->Content.block_data_transfer.write_back_bit = 0;
		if (!strcmp(rn_string, "sp")) { token->Content.block_data_transfer.rn = 13; }
		else {
			token->Content.block_data_transfer.rn = strtol(rn_string + 1, NULL, 10);
		}
		copy_string1 += 3;
	}

	copy_string1++;
	char *p;
	memset(token->Content.block_data_transfer.register_list, 0, 16 * sizeof(uint8_t));
	p = strtok(copy_string1, ",");
	while (p) {
		int reg = atoi(p + 1);
		token->Content.block_data_transfer.register_list[reg] = 1;
		p = strtok(NULL, ",");
	}

	switch (token->opcode) {
	case LDM:
		switch (token->Content.block_data_transfer.characteristic) {
		case FD:
			token->Content.block_data_transfer.pre_post_index = 0;
			token->Content.block_data_transfer.up_down_bit = 1;
			token->Content.block_data_transfer.load_store_bit = 1;
			break;
		case ED:
			token->Content.block_data_transfer.pre_post_index = 1;
			token->Content.block_data_transfer.up_down_bit = 1;
			token->Content.block_data_transfer.load_store_bit = 1;
			break;
		case FA:
			token->Content.block_data_transfer.pre_post_index = 0;
			token->Content.block_data_transfer.up_down_bit = 0;
			token->Content.block_data_transfer.load_store_bit = 1;
			break;
		case EA:
			token->Content.block_data_transfer.pre_post_index = 1;
			token->Content.block_data_transfer.up_down_bit = 0;
			token->Content.block_data_transfer.load_store_bit = 1;
			break;
		default:
			break;
		}
		break;
	case STM:
		switch (token->Content.block_data_transfer.characteristic) {
		case FD:
			token->Content.block_data_transfer.pre_post_index = 1;
			token->Content.block_data_transfer.up_down_bit = 0;
			token->Content.block_data_transfer.load_store_bit = 0;
			break;
		case ED:
			token->Content.block_data_transfer.pre_post_index = 0;
			token->Content.block_data_transfer.up_down_bit = 0;
			token->Content.block_data_transfer.load_store_bit = 0;
			break;
		case FA:
			token->Content.block_data_transfer.pre_post_index = 1;
			token->Content.block_data_transfer.up_down_bit = 1;
			token->Content.block_data_transfer.load_store_bit = 0;
			break;
		case EA:
			token->Content.block_data_transfer.pre_post_index = 0;
			token->Content.block_data_transfer.up_down_bit = 1;
			token->Content.block_data_transfer.load_store_bit = 0;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

static void parse_transfer(Token *token, const char *string_transfer) {
	token->Content.transfer.rd = (unsigned int) atoi(string_transfer + 1);
	char *copy_transfer = calloc(strlen(string_transfer) + 1, sizeof(char));
	strcpy(copy_transfer, string_transfer);
	char *copy_transfer_split = split(copy_transfer, ',', 1);
	char *skip_spaces = skip_whitespace(copy_transfer_split);
	token->Content.transfer.address = parse_address(skip_spaces, token);

	free(copy_transfer);
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
static int parse_expression(char *expression) {
	if (expression[0] == '[') {
		expression++;
	}
	if (expression[strlen(expression) - 1] == ']') {
		expression[strlen(expression) - 1] = '\0';
	}
	if (!strncmp(expression, "-", 1)) {
		return -parse_expression(expression + 1);
	}

	if (!strncmp(expression, "0x", 2)) {      //HEX
		return strtol(expression + 2, NULL, 16);
	} else { return strtol(expression, NULL, 10); }      //DECIMAL
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
	printf("String_to_shift exit");
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
	// char *args = strtok(instruction, " ");
	char *args = split(instruction, ' ', 1);
	mnemonic_t operation;
	if (!strncmp(instruction, "ldm", 3) || !strncmp(instruction, "stm", 3)) {
		operation = string_to_mnemonic_stack(instruction, token);
	} else {
		operation = string_to_mnemonic(instruction);
	}

	token->opcode = operation;
	token->flag = 0;
	token->condition = AL;
	args = skip_whitespace(args);

	if (operation == LSL) {
		parse_special(token, args);

	} else if (operation == ANDEQ) {
		token->condition = EQ;
		parse_special(token, args);

	} else if (operation <= CMP) {
		parse_data_processing(token, args);

	} else if (operation <= MLA) {
		parse_multiply(token, args);

	} else if (operation <= STR) {
		parse_transfer(token, args);

	} else if (operation <= B) {
		parse_branch(token, args);

	} else if (operation <= STM) {
		parse_block_data_transfer(token, args);
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
		copy_string++;
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
			if (counter == number_occurrence) break;
		}
		i++;
	}
	pointers[i] = '\0';
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
	tokens[number_tokens] = strtok(pointers, separator);
	while (tokens[number_tokens] != NULL &&
	       number_tokens != number_occurrences) {
		number_tokens++;
		tokens[number_tokens] = split(tokens[number_tokens - 1], ',', 1);
	}
	return tokens;
}
