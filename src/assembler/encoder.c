#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "define_types.h"
#include "encoder.h"



#define COND_POS 28
#define IMMEDIATE_POS 25
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
    *binary |= (input << pos);
}

void instr_to_bits(Token *token,label_dict *dict, uint16_t address,uint32_t *binary) {
    *binary = 0;
    to_bits(binary, token->condition, COND_POS);
    if (token->opcode <= CMP) {
        data_proc_to_bits(token, binary);
    }

    if (token->opcode <= MLA) {
         mul_to_bits(token, binary);
    }

    if (token->opcode <= STR) {
         data_transfer_to_bits(token, address,binary);
    }

    if (token->opcode <= B) {
         branch_to_bits(token, binary,dict);
    }
    if (token->opcode <= ANDEQ) {
         special_to_bits(token, binary, dict);
    }
    printf("Invalid Token opcode passed to be encoded\n");
    exit(EXIT_FAILURE);
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
    (token->data_processing.op2.immediate) ? to_bits(binary, 1, IMMEDIATE_POS) : (binary = 0);
    uint8_t opcode;
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
        default:
            printf("Invalid Token opcode passed to be encoded\n");
            exit(EXIT_FAILURE);
    }
    to_bits(binary, opcode, OPCODE_POS);
    (token->opcode == TST || token->opcode == TEQ || token->CMP) ? to_bits(instr, 1, SET_COND_POS) : to_bits(
            binary, token->data_processing.rd, RD_POS);
    if (token->opcode != MOV) {
        to_bits(binary, token->data_processing.rn, RN_POS);
    }
    if (token->data_processing.op2.immediate) {
        int exp = token->data_processing.op2.expression;
        if (exp > 0xff) {
            int amount = 0;
            uint32_t rm_value = (uint32_t) token->data_processing.op2.shifted_register.rm;
            while (rm_value > 0xff) {
                amount++;
                rm_value = rotate_right(rotate_right, 2);
            }
            to_bits(binary, rm_value, OP2_POS);
            to_bits(binary, (uint32_t) amount, 8);
        } else {
            to_bits(binary, (uint32_t) exp, OP2_POS);
        }
    } else {
        to_bits(binary, (uint32_t) token->data_processing.op2.shifted_register.rm, OP2_POS);
        if (!token->data_processing.op2.shifted_register.shift.format) { //register
            shift_t type = token->data_processing.op2.shifted_register.shift.type;
            to_bits(binary, (uint32_t) type, SHIFT_T_POS);
            if (!(token->data_processing.op2.shifted_register.shift.type == NO_SHIFT)) {
                to_bits(binary, 1, 4);
                to_bits(binary, token->data_processing.op2.shifted_register.shift.regist, 8);
            }
        }
    }
}


void mul_to_bits(Token *token, uint32_t *binary) {
    assert(token != NULL);
    to_bits(binary, 9, 4); //for 1001 in bits 4-7 in mul instruction
    to_bits(binary, (uint32_t) token->multiply.rd, RD_POS_MUL);
    to_bits(binary, (uint32_t) token->multiply.rn, RN_POS_MUL);
    to_bits(binary, (uint32_t) token->multiply.rs, RS_POS_MUL);
    to_bits(binary, (uint32_t) token->multiply.rm, RM_POS_MUL);
    if (token->opcode == MLA) {
        to_bits(binary, 1, ACCUMULATE_POS); //set accumulate bit
    }
}

void data_transfer_to_bits(Token *token, uint32_t *binary, uint16_t address) {
    assert(token != NULL);
    to_bits(binary, 1, 26); // bit 26 is set for data transfer
    to_bits(binary, (uint32_t) token->transfer.rd, RD_POS);
    if (token->opcode == LDR) {
        to_bits(binary, 1, LOAD_POS); //set load bit
    }
    //TODO
}

void branch_to_bits(Token *token, uint32_t *binary, label_dict *dict) {
    assert(token != NULL);
    to_bits(binary, 0xa, BRANCH_BITS_POS); //for 1010 in buts 24-27 in branch instruction
    int label_address = query("label",dict) * 4;
    int diff = label_address - ((int) token->address) - 8; //-8 because of the ARM pipeline
    diff >>= 2;
    diff &= 0x00ffffff; //all 1s
    to_bits(binary, (uint32_t) diff, 0);
}

void special_to_bits(Token *token, uint32_t *binary,label_dict *dict) { //consider lsl rn <#exp> = mov rn, rn, <#exp>
    assert(token != NULL);
    uint32_t opcode;
    opcode = 0xd; //1101
    to_bits(binary, opcode, 21);
    to_bits(binary, (uint32_t) token->data_processing.rd, RD_POS);
    //mov rn, rn, lsl <#exp>
    to_bits(binary, (uint32_t) token->data_processing.rd, 0);
    to_bits(binary, (uint32_t) token->data_processing.op2.expression, 7);
}

void address_to_bits(Address address, uint32_t *binary) {
    to_bits(binary, (uint32_t) address.Register.rn, RN_POS); // base register

    if (address.Register.format == 0) {
        if (address.Register.expresion >= 0) {
            to_bits(binary, 1, UP_POS);
            to_bits(binary, (uint32_t) address.Register.expression, OFFSET_POS);
        } else {
            to_bits(binary, (uint32_t)(-1 * address.Register.expression),
                    OFFSET_POS);
        }
    } else { //op2 = register
        //set flags
        to_bits(binary, address.Register.Shift.rm, OFFSET_POS);
        to_bits(binary, 1, IMMEDIATE_POS);
        switch (address.Register.Shift.shift.type) {   //to_bits the type of rotation, if one exists
            case ASR_SHIFT:
                to_bits(binary, 2, SHIFT_T_POS);
                break;
            case LSL_SHIFT:
                to_bits(binary, 0, SHIFT_T_POS);
                break;
            case LSR_SHIFT:
                to_bits(binary, 1, SHIFT_T_POS);
                break;
            case ROR_SHIFT:
                to_bits(binary, 3, SHIFT_T_POS);
                break;
            default:
                break;
        }
        if (address.Register.Shift.pm == 0) { // positive offset
            to_bits(binary, 1, UP_POS);
        }
        if (address.Register.Shift.shift.format == 0 &&
            address.Register.Shift.shift.type !=
            NO_SHIFT) {           // shift specified by another register
            to_bits(binary, 1, 4);
            to_bits(binary, address.Register.Shift.shift.regist, SHIFT_REG_POS);
        } else if (address.Register.Shift.shift.type !=
                   NO_SHIFT &&
                   address.Register.Shift.shift.format == 1) {  //shift specified by a constant
            assign_bits(binary, address.Register.Shift.shift.expression, SHIFT_CONST_POS);
        }
    }
}
