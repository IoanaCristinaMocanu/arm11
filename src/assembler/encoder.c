#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "define_types.h"
#include "encoder.h"


#define COND_POS 28
#define IMMEDIATE_POS 25
#define PRE_POS 24
#define UP_POS 23
#define OPCODE_POS 21
#define SET_COND_POS 20
#define RD_POS 12
#define RN_POS 16
#define OP2_POS 0
#define RM_POS_MUL 0
#define RS_POS_MUL 8
#define RN_POS_MUL 12
#define RD_POS_MUL 16
#define ACCUMULATE_POS 21
#define LOAD_POS 20
#define SHIFT_T_POS 5
#define SHIFT_REG_POS 8
#define SHIFT_CONST_POS 7
#define BRANCH_BITS_POS 24
#define OFFSET_POS 0
#define UP_POS 23
#define SHIFT_REG_POS 8
#define SHIFT_CONST_POS 7

static void to_bits(uint32_t *binary, uint32_t input, int pos) {
    (*binary) |= (input << pos);
}

void instr_to_bits(Token *token, label_dict *dict, uint16_t address, uint32_t *binary) {
    *binary = 0;
    to_bits(binary, token->condition, COND_POS);
    if (token->opcode <= CMP) {
        data_proc_to_bits(token, binary);
    } else if (token->opcode <= MLA) {
        mul_to_bits(token, binary);
    } else if (token->opcode <= STR) {
        data_transfer_to_bits(token, address, binary);
    } else if (token->opcode <= B) {
        branch_to_bits(token, binary, dict);
    } else if (token->opcode <= ANDEQ) {
        special_to_bits(token, binary, dict);
    } else {
        printf("Invalid Token opcode passed to be encoded\n");
        exit(EXIT_FAILURE);
    }
}


uint32_t rotate_right(uint32_t to_shift, uint8_t ammount) {
    ammount = ammount % 32;
    uint32_t bottom = to_shift >> ammount;
    uint8_t offset = 32 - ammount;
    uint32_t top = (to_shift << offset);
    return top | bottom;
}

void data_proc_to_bits(Token *token, uint32_t *binary) {
    assert(token != NULL);
    if (token->Content.data_processing.op2.immediate) {
        to_bits(binary, (uint32_t) 1, IMMEDIATE_POS);
    } else { *binary = 0x0; }
    uint32_t opcode;
    switch (token->opcode) {
        case ANDEQ:
        case AND:
            opcode = 0x0;
            break;
        case EOR:
            opcode = 0x1;
            break;
        case SUB:
            opcode = 0x2;
            break;
        case RSB:
            opcode = 0x3;
            break;
        case ADD:
            opcode = 0x4;
            break;
        case TST:
            opcode = 0x8;
            break;
        case TEQ:
            opcode = 0x9;
            break;
        case CMP:
            opcode = 0xa;
            break;
        case ORR:
            opcode = 0xc;
            break;
        case MOV:
            opcode = 0xd;
            break;
        default: {
            printf("Invalid Token opcode passed to be encoded\n");
            exit(EXIT_FAILURE);
        }
    }
    to_bits(binary, opcode, OPCODE_POS);
    to_bits(binary, 0xe, 28);
    (token->opcode == TST || token->opcode == TEQ || token->opcode == CMP)
    ? to_bits(binary, 1, SET_COND_POS)
    : to_bits(binary, (int) token->Content.data_processing.rd, RD_POS);
    if (token->opcode != MOV) {
        to_bits(binary, token->Content.data_processing.rn, RN_POS);
    }
    if (token->Content.data_processing.op2.immediate) {
        int exp = token->Content.data_processing.op2.Register.expression;
        printf("expression in data proc: %d", exp);
        if (exp > 0xff) {
            if (token->opcode == MOV) {
                exp = convertOP2(exp);
                to_bits(binary, (uint32_t) exp, OP2_POS);
            } else {
                int amount = 0;
                uint32_t rm_value = (uint32_t) token->Content.data_processing.op2.Register.shifted_register.rm;
                printf("rm value %d", (uint32_t) token->Content.data_processing.op2.Register.shifted_register.rm);
                while (rm_value > 0xff) {
                    amount++;
                    rm_value = rotate_right(rm_value, 2);
                }
                to_bits(binary, rm_value, OP2_POS);
                to_bits(binary, (uint32_t) amount, 8);
            }
        } else {
            to_bits(binary, (uint32_t) exp, OP2_POS);
        }
        printf("Valoare exp in encoder: %d\n", exp);
    } else {
        to_bits(binary, (uint32_t) token->Content.data_processing.op2.Register.shifted_register.rm, OP2_POS);
        if (!token->Content.data_processing.op2.Register.shifted_register.shift.format) {
            shift_t type = token->Content.data_processing.op2.Register.shifted_register.shift.type;
            to_bits(binary, (uint32_t) type, SHIFT_T_POS);
            if (!(token->Content.data_processing.op2.Register.shifted_register.shift.type == NO_SHIFT)) {
                to_bits(binary, 1, 4);
                to_bits(binary, token->Content.data_processing.op2.Register.shifted_register.shift.args.regist, 8);
            }
        }
    }
}


void mul_to_bits(Token *token, uint32_t *binary) {
    assert(token != NULL);
    to_bits(binary, 9, 4);   //for 1001 in bits 4-7 in mul instruction
    to_bits(binary, (uint32_t) token->Content.multiply.rd, RD_POS_MUL);
    printf("mul_to_bits print:\n");
    printf("%x", token->Content.multiply.rd);
    printf("\n");
    printf("%x", token->Content.multiply.rn);
    printf("\n");
    printf("%x", token->Content.multiply.rs);
    printf("\n");
    printf("%x", token->Content.multiply.rm);
    printf("\n");
    printf("%x\n", *binary);
    to_bits(binary, (uint32_t) token->Content.multiply.rn, RN_POS_MUL);
    printf("%x\n", *binary);
    to_bits(binary, (uint32_t) token->Content.multiply.rs, RS_POS_MUL);
    printf("%x\n", *binary);
    to_bits(binary, (uint32_t) token->Content.multiply.rm, RM_POS_MUL);
    printf("%x\n", *binary);
    if (token->opcode == MLA) {
        to_bits(binary, 1, ACCUMULATE_POS);     //set accumulate bit
    }
}

void data_transfer_to_bits(Token *token, uint16_t address, uint32_t *binary) {
    assert(token != NULL);
    to_bits(binary, 1, 26);   // bit 26 is set for data transfer
    to_bits(binary, (uint32_t) token->Content.transfer.rd, RD_POS);
    if (token->opcode == LDR) {
        to_bits(binary, 1, LOAD_POS);     //set load bit
    }
    if (token->Content.transfer.address.format == 0) { //numeric constant expression
        int expression = token->Content.transfer.address.Expression.expression;
        if (expression < 0xff) {
            Token token_MOV;                          //TOKEN FOR MOV
            token_MOV.opcode = MOV;
            token_MOV.Content.data_processing.rd = token->Content.transfer.rd;
            token_MOV.Content.data_processing.op2.immediate = 1;
            token_MOV.Content.data_processing.op2.Register.expression
                    = token->Content.transfer.address.Expression.expression;
            *binary = 0;
            to_bits(binary, AL, 28);
            data_proc_to_bits(&token_MOV, binary);
        } else {
            int offset = ((int) address) - ((int) token->address) - 8;      //PC is ahead with 8 bytes of instructions
            to_bits(binary, 15, RN_POS);
            to_bits(binary, 1, PRE_POS);
            to_bits(binary, 1, UP_POS);
            to_bits(binary, (uint32_t) offset, OFFSET_POS);
        }
    } else {
        if (!token->Content.transfer.address.Expression.Register.pre_post_index)
            to_bits(binary, 1, PRE_POS);
        address_to_bits(token->Content.transfer.address, binary);
    }
}

void branch_to_bits(Token *token, uint32_t *binary, label_dict *dict) {
    assert(token != NULL);
    to_bits(binary, 0xa, BRANCH_BITS_POS);   //for 1010 in buts 24-27 in branch instruction
    int label_address = query(token->Content.branch.expression, dict) * 4;
    int diff = label_address - ((int) token->address) - 8;   //-8 because of the ARM pipeline
    diff >>= 2;
    diff &= 0x00ffffff;   //all 1s
    to_bits(binary, (uint32_t) diff, 0);
}

void special_to_bits(Token *token, uint32_t *binary, label_dict *dict) { //consider lsl rn <#exp> = mov rn, rn, <#exp>
    assert(token != NULL);
    uint32_t opcode = 0xd;   //1101
    to_bits(binary, opcode, 21);
    to_bits(binary, (uint32_t) token->Content.data_processing.rd, RD_POS);
    //mov rn, rn, lsl <#exp>
    to_bits(binary, (uint32_t) token->Content.data_processing.rd, 0);
    to_bits(binary, (uint32_t) token->Content.data_processing.op2.Register.expression, 7);
}

void address_to_bits(Address address, uint32_t *binary) {
    to_bits(binary, (uint32_t) address.Expression.Register.rn, RN_POS);   // base register

    if (address.Expression.Register.format == 0) {
        if (address.Expression.expression >= 0) {
            to_bits(binary, 1, UP_POS);
            to_bits(binary, (uint32_t) address.Expression.expression, OFFSET_POS);
        } else {
            to_bits(binary, (uint32_t)(-1 * address.Expression.expression),
                    OFFSET_POS);
        }
    } else {   //op2 = register
        //set flags
        to_bits(binary, address.Expression.Register.Offset.Shift.rm, OFFSET_POS);
        to_bits(binary, 1, IMMEDIATE_POS);
        switch (address.Expression.Register.Offset.Shift.shift.type) {       //to_bits the type of rotation, if one exists
            case SHIFT_ASR:
                to_bits(binary, 2, SHIFT_T_POS);
                break;
            case SHIFT_LSL:
                to_bits(binary, 0, SHIFT_T_POS);
                break;
            case SHIFT_LSR:
                to_bits(binary, 1, SHIFT_T_POS);
                break;
            case SHIFT_ROR:
                to_bits(binary, 3, SHIFT_T_POS);
                break;
            default:
                break;
        }
        if (address.Expression.Register.Offset.Shift.pm == 0) {     // positive offset
            to_bits(binary, 1, UP_POS);
        }
        if (address.Expression.Register.Offset.Shift.shift.format == 0 &&
            address.Expression.Register.Offset.Shift.shift.type !=
            NO_SHIFT) {               // shift specified by another register
            to_bits(binary, 1, 4);
            to_bits(binary, address.Expression.Register.Offset.Shift.shift.args.regist, SHIFT_REG_POS);
        } else if (address.Expression.Register.Offset.Shift.shift.type !=
                   NO_SHIFT &&
                   address.Expression.Register.Offset.Shift.shift.format == 1) {      //shift specified by a constant
            to_bits(binary, address.Expression.Register.Offset.Shift.shift.args.expression, SHIFT_CONST_POS);
        }
    }
}

int convertOP2(int op2) {
    uint32_t rot, result = op2;
    int condition = 1, rotate_value = 0;
    while (condition) {
        /*check bit0 and bit1*/
        int bit0 = (op2 & 1);
        int bit1 = ((op2 & 2) >> 1);
        /*stop the while loop either bit0 or bit1 is 1*/
        if ((bit0) || (bit1)) {
            condition = 0;
        }
        if (condition) {
            /*shift value = n -> shift 2n times*/
            op2 = op2 >> 2;
            rotate_value++;
        }
    }
    if (rotate_value != 0) {
        /*count rotate backward (1 round is 16)*/
        rot = (uint32_t)(16 - rotate_value);
        /*rot is the shift value which starts at bit 8*/
        result = op2 | (rot << 8);
    }
    return result;
}
