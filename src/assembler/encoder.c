#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "define_structures.h"
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

static void to_bits(Instr *instruction, uint32_t input, int pos) {
    *instruction |= (input << pos);
}

Instr instr_to_bits(Token *token, symbol_table, unsigned int address) {
    Instr instruction = 0;
    to_bits(&instruction, token.condition, COND_POS);
    if (token.opcode <= CMP) {
        return data_proc_to_bits(token, &instruction);
    }

    if (token.opcode <= MLA) {
        return mul_to_bits(token, &instruction);
    }

    if (token.opcode <= STR) {
        return data_transfer_to_bits(token, &instruction, address);
    }

    if (token.opcode <= B) {
        return branch_to_bits(token, &instruction);
    }
    if (token.opcode <= ANDEQ) {
        return special_to_bits(token, &instruction, symbol_table);
    }
    print("Invalid Token opcode passed to be encoded\n");
    exit(EXIT_FAILURE);
}

uint32_t rotate_right(uint32_t to_shift, uint8_t ammount) {
    ammount = ammount % 32;
    uint32_t bottom = to_shift >> ammount;
    uint8_t offset = 32 - ammount;
    uint32_t top = (to_shift << offset);
    return top | bottom;
}

Instr *data_proc_to_bits(Token *token, Instr *instruction) {
    assert(token != NULL);
    (instruction->data_processing.op2.immediate) ? to_bits(instruction, 1, IMMEDIATE_POS) : (*instruction = 0);
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
            print("Invalid Token opcode passed to be encoded\n");
            exit(EXIT_FAILURE);
    }
    to_bits(instruction, opcode, OPCODE_POS);
    (token->opcode == TST || token->opcode == TEQ || token->CMP) ? to_bits(instr, 1, SET_COND_POS) : to_bits(
            instruction, token->data_processing.rd, RD_POS);
    if (token->opcode != MOV) {
        to_bits(instruction, token->data_processing.rn, RN_POS);
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
            to_bits(instruction, rm_value, OP2_POS);
            to_bits(instruction, (uint32_t) amount, 8);
        } else {
            to_bits(instruction, (uint32_t) exp, OP2_POS);
        }
    } else {
        to_bits(instr, (uint32_t) token->data_processing.op2.shifted_register.rm, OP2_POS);
        if (!token->data_processing.op2.shifted_register.shift.format) { //register
            shift_t type = token->data_processing.op2.shifted_register.shift.type;
            to_bits(instruction, (uint32_t) type, SHIFT_T_POS);
            if (!(token->data_processing.op2.shifted_register.shift.type == NO_SHIFT)) {
                to_bits(instruction, 1, 4);
                to_bits(instruction, token->data_processing.op2.shifted_register.shift.regist, 8);
            }
        }
    }
    return instruction;
}


Instr *mul_to_bits(Token *token, Instr *instruction) {
    assert(token != NULL);
    to_bits(instruction, 9, 4); //for 1001 in bits 4-7 in mul instruction
    to_bits(instruction, (uint32_t) token->multiply.rd, RD_POS_MUL);
    to_bits(instruction, (uint32_t) token->multiply.rn, RN_POS_MUL);
    to_bits(instruction, (uint32_t) token->multiply.rs, RS_POS_MUL);
    to_bits(instruction, (uint32_t) token->multiply.rm, RM_POS_MUL);
    if (token->opcode == MLA) {
        to_bits(instruction, 1, ACCUMULATE_POS); //set accumulate bit
    }
    return instruction;
}

Instr *data_transfer_to_bits(Token *token, Instr *instruction, unsigned int address) {
    assert(token != NULL);
    to_bits(instruction, 1, 26); // bit 26 is set for data transfer
    to_bits(instruction, (uint32_t) token->transfer.rd, RD_POS);
    if (token->opcode == LDR) {
        to_bits(instruction, 1, LOAD_POS); //set load bit
    }
    //TODO
}

Instr *branch_to_bits(Token *token, Instr *instruction, symbol_table) {
    assert(token != NULL);
    to_bits(instruction, 0xa, BRANCH_BITS_POS); //for 1010 in buts 24-27 in branch instruction
    int label_address; //TODO: = lookup in symbol table for branch exp
    int diff = label_address - ((int) token->address) - 8; //-8 because of the ARM pipeline
    diff >>= 2;
    diff &= 0x00ffffff; //all 1s
    to_bits(instruction, (uint32_t) diff, 0);
    return instruction;
}

Instr *special_to_bits(Token *token, Instr *instruction) { //consider lsl rn <#exp> = mov rn, rn, <#exp>
    assert(token != NULL);
    uint32_t opcode;
    opcode = 0xd; //1101
    to_bits(instruction, opcode, 21);
    to_bits(instruction, (uint32_t) token->data_processing.rd, RD_POS);
    //mov rn, rn, lsl <#exp>
    to_bits(instruction, (uint32_t) token->data_processing.rd, 0);
    to_bits(instruction, (uint32_t) token->data_processing.op2.expression, 7);
    return instruction;
}

Instr *address_to_bits(Address address, Instr *instruction) {
    to_bits(instruction, (uint32_t) address.Register.rn, RN_POS); // base register

    if (address.Register.format == 0) {
        if (address.Register.expresion >= 0) {
            to_bits(instruction, 1, UP_POS);
            to_bits(instruction, (uint32_t) address.Register.expression, OFFSET_POS);
        } else {
            to_bits(instruction, (uint32_t)(-1 * address.Register.expression),
                    OFFSET_POS);
        }
    } else { //op2 = register
        //set flags
        to_bits(instruction, address.Register.Shift.rm, OFFSET_POS);
        to_bits(instruction, 1, IMMEDIATE_POS);
        switch (address.Register.Shift.shift.type) {   //to_bits the type of rotation, if one exists
            case ASR_SHIFT:
                to_bits(instr, 2, SHIFT_T_POS);
                break;
            case LSL_SHIFT:
                to_bits(instr, 0, SHIFT_T_POS);
                break;
            case LSR_SHIFT:
                to_bits(instr, 1, SHIFT_T_POS);
                break;
            case ROR_SHIFT:
                to_bits(instr, 3, SHIFT_T_POS);
                break;
            default:
                break;
        }
        if (address.Register.Shift.pm == 0) { // positive offset
            to_bits(instruction, 1, UP_POS);
        }
        if (address.Register.Shift.shift.format == 0 &&
            address.Register.Shift.shift.type !=
            NO_SHIFT) {           // shift specified by another register
            to_bits(instruction, 1, 4);
            to_bits(instruction, address.Register.Shift.shift.regist, SHIFT_REG_POS);
        } else if (address.Register.Shift.shift.type !=
                   NO_SHIFT &&
                   address.Register.Shift.shift.format == 1) {  //shift specified by a constant
            assign_bits(instruction, address.Register.Shift.shift.expression, SHIFT_CONST_POS);
        }
    }
}