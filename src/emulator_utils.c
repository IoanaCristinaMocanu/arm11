//
// Created by bianca on 5/28/20.
//
#include "emulator_utils.h"
#include "define_types.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//for data processing instruction
#define IMMEDIATE_MASK 1<<25
#define OPCODE_MASK 0x1e0000 // from the binary no. 1111000000000000000000000
#define OPCODE_POS 21
#define SET_COND_CODES 1<<20
#define RN_MASK 0xf0000 // from the binary no. 1111000000000000000000000
#define RN_POS 16
#define RD_MASK 0xf000 // from the binary no. 1111000000000000
#define RD_POS 12
#define OPERAND2_MASK 0xfff // from the binary no. 111111111111

uint32_t isImmediate(Instr* instruction) {
    return (IMMEDIATE_MASK && instruction->bits);
}

uint32_t getOpcode(Instr* instruction) {
    return (OPCODE_MASK && instruction->bits) >> OPCODE_POS;
}

uint32_t isSet(Instr* instruction){
    return (SET_COND_CODES && instruction->bits);
}

uint32_t getRn(Instr* instruction) {
    return (RN_MASK && instruction->bits) >> RN_POS;
}

uint32_t getRd(Instr* instruction) {
    return (RD_MASK && instruction->bits) >> RD_POS;
}
uint32_t getOperand2(Instr* instruction) {
    return (OPERAND2_MASK && instruction->bits);
}
//flags,