//
// Created by bianca on 5/28/20.
//
#include "decode_helpers.h"
#include "define_structures.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//FUNCTIONS:

//FOR ALL TYPES OF INSTRUCTIONS
//return condition bits
uint32_t getCond(Instr *instruction) {
    return (instruction->bits) >> COND_POS;
}

//return condition type
enum type getInstrType(Instr *instruction) {

    if ((1 << 27) & instruction->bits) {
        return BRANCH;
    }
    if ((1 << 26) & instruction->bits) {
        return TRANSFER;
    }
    if (IMMEDIATE_MASK & instruction->bits) {
        return DATA_PROC;
    }
    if (!((1 << 4) & instruction->bits)) {
        return DATA_PROC;
    }
    if (!((1 << 7) & instruction->bits)) {
        return DATA_PROC;
    }
    return MUL;
}


//FOR DATA PROCESSING ONLY
//return Opcode
uint32_t getOpcode(Instr *instruction) {
    return (OPCODE_MASK & instruction->bits) >> OPCODE_POS;
}

//return Operand2
uint32_t getOperand2(Instr *instruction) {
    return (OPERAND2_MASK & instruction->bits);
}

//FOR MULTIPLY ONLY
//check if it should accumulate
bool toAccumulate(Instr *instruction) {
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
    return (RM_MUL_MASK & instruction->bits);
}


//FOR SINGLE DATA TRANSFER ONLY
//check Pre/Post-indexing bit
bool isP_indexing(Instr *instruction) {
    return (P_INDEXING_MASK & instruction->bits);
}

//check Up bit
bool isUp(Instr *instruction) {
    return (UP_MASK & instruction->bits);
}

//check Load bit
bool isLoad(Instr *instruction) {
    return (LOAD_MASK & instruction->bits);
}

//getOffset
uint32_t getOffset_TRANSFER(Instr *instruction) {
    return (OFFSET_TRANSFER_MASK & instruction->bits);
}


//FOR BRANCH ONLY
//get Offset
uint32_t getOffset_BRANCH(Instr *instruction) {
    return (OFFSET_BRANCH_MASK & instruction->bits);
}

//FOR DATA PROCESSING AND MULTIPLY ONLY
//check Immediate bit
bool isImmediate(Instr *instruction) {
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
bool isSet(Instr *instruction) {
    return (SET_COND_CODES & instruction->bits);
}




