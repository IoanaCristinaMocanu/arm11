//
// Created by bianca on 5/28/20.
//
#include "emulator_utils.h"
#include "define_types.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//CONSTANTS:

//for all types of instruction
#define COND_POS 28
#define N_MASK 1 << 31
#define Z_MASK 1 << 30
#define C_MASK 1 << 29
#define V_MASK 1 << 28

//for Data Processing ONLY
#define OPERAND2_MASK 0xfff // for bits 0-11
#define OPCODE_MASK 0x1e0000 //for bits 21-24
#define OPCODE_POS 21

//for Multiply ONLY
#define ACCUMULATE 1 << 21
#define RD_MUL_MASK 0xf0000 //for bits 16-19
#define RN_MUL_MASK 0xf000 //for bits 12-15
#define RS_MUL_MASK 0xf00 // for bits 8-11
#define RM_MUL_MASK 15 //for bits 0-3
#define RD_MUL_POS 16
#define RN_MUL_POS 12
#define RS_MUL_POS 8

//for Single Data Transfer ONLY
#define P_INDEXING_MASK 1 << 24
#define UP_MASK 1 << 23
#define LOAD_MASK 1 << 20

//for Data Processing and Single Data Transfer
#define IMMEDIATE_MASK 1 << 25
#define RN_MASK 0xf0000 //for bits 16-19
#define RD_MASK 0xf000 //for bits 12-15
#define RN_POS 16
#define RD_POS 12

//for Data Processing and Multiply
#define SET_COND_CODES 1 << 20


//FUNCTIONS:

//FOR DATA PROCESSING ONLY
//return Opcode
uint32_t getOpcode(Instr *instruction) {
    return (OPCODE_MASK & instruction->bits) >> OPCODE_POS;
}

//return Operand2
uint32_t getOperand2(Instr *instruction) {
    return (OPERAND2_MASK & instruction->bits);
}

//check if the Cond field is satisfied by the CPSR register
bool check_Condition(Machine *arm, Instr *instruction) {
    uint32_t cond = getCond(instruction);
    uint32_t cpsr = arm->cpsr_reg;
    uint32_t get_N = N_MASK && cpsr;
    uint32_t get_Z = Z_MASK && cpsr;
    uint32_t get_V = V_MASK && cpsr;

    switch (cond) {
        case eq:
            return get_Z;
        case ne:
            return !get_Z;
        case ge:
            return (get_N == get_V);
        case lt:
            return !(get_N == get_V);
        case gt:
            return !get_Z && (get_N == get_V);
        case le:
            return get_Z || !(get_N == get_V);
        case al:
            return true;
        default:
            return false;
    }
}

//set CPSR flags
void set_flags(Machine *arm, uint8_t N, uint8_t Z, uint8_t C, uint8_t V) {
    N ? (*arm->cpsr_reg |= N_MASK) : (*arm->cpsr_reg &= ~(N_MASK));
    Z ? (*arm->cpsr_reg |= Z_MASK) : (*arm->cpsr_reg &= ~(Z_MASK));
    C ? (*arm->cpsr_reg |= C_MASK) : (*arm->cpsr_reg &= ~(C_MASK));
    V ? (*arm->cpsr_reg |= V_MASK) : (*arm->cpsr_reg &= ~(V_MASK));
}


//FOR MULTIPLY ONLY
//check if it should accumulate
uint32_t toAccumulate(Instr *instruction) {
    return (ACCUMULATE & instruction->bits);
}

//return Rn
uint32_t getRn_MUL(Instr *instruction) {
    return (RN_MUL_MASK & instruction->bits) >> RN_MUL_POS;
}

// return Rd
uint32_t getRd_MUL(Instr *instruction) {
    return (RD_MUL_MASK & instruction->bits) >> RD_MUL_POS;
}

//return Rs
uint32_t getRs_MUL(Instr *instruction) {
    return (RS_MUL_MASK & instruction->bits) >> RS_MUL_POS;
}

//Return Rm
uint32_t getRm_MUL(Instr *instruction) {
    return (RM_MUL_MASK & instruction->bits) >> RM_MUL_POS;
}


//FOR SINGLE DATA TRANSFER ONLY
//check Pre/Post-indexing bit
uint32_t isP_indexing(Instr *instruction) {
    return (P_INDEXING_MASK & instruction->bits);
}

//check Up bit
uint32_t isUp(Instr *instruction) {
    return (UP_MASK & instruction->bits);
}

//check Load bit
uint32_t isLoad(Instr *instruction) {
    return (LOAD_MASK & instruction->bits);
}


//FOR DATA PROCESSING AND MULTIPLY ONLY
//check Immediate bit
uint32_t isImmediate(Instr *instruction) {
    return (IMMEDIATE_MASK & instruction->bits);
}

//return Rn
uint32_t getRn(Instr *instruction) {
    return (RN_MASK & instruction->bits) >> RN_POS;
}

//return Rd
uint32_t getRd(Instr *instruction) {
    return (RD_MASK & instruction->bits) >> RD_POS;
}


//FOR DATA PROCESSING AND SINGLE DATA TRANSFER ONLY
//check Set Condition Codes bit
uint32_t isSet(Instr *instruction) {
    return (SET_COND_CODES & instruction->bits);
}


//FOR ALL TYPES OF INSTRUCTIONS
//return condition bits
uint32_t getCond(Instr *instruction) {
    return (instruction->bits) >> COND_POS;
}

//return condition type
enum type getIstrType(Instr *instruction) {
    if (instruction == NOOP) {
        return NOOP;
    }
    if ((1 << 27) & instruction->bits) {
        return BRANCH;
    }
    if ((1 << 26) & instruction->bits) {
        return TRANSFER;
    }
    if (IMMEDIATE_MASK & instruction->bits) {
        return DATA_PROC;
    }
    if (!((1 << 7) & instruction->bits)) {
        return DATA_PROC;
    }
    if (!((1 << 4) & instruction->bits)) {
        return DATA_PROC;
    }
    return MUL;
}

}



