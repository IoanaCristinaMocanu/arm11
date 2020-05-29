#include "emulator_processor.h"
#include "decode_helpers.c"

#define N_MASK 1 << 31
#define Z_MASK 1 << 30
#define C_MASK 1 << 29
#define V_MASK 1 << 28

void execute(Decoded_Instr *instr, Machine *arm) {

}


void decode(Decoded_Instr *decoded, Instr *instr, Machine *arm) {
    decoded->type = getInstrType(instr);
    decoded->exists = true;
    decoded->cond = getCond(instr);
    if (decoded->type == DATA_PROC) {
        decoded->immediate = isImmediate(instr);
        decoded->opcode = getOpcode(instr);
        decoded->set = isSet(instr);
        decoded->rn = getRn(instr);
        decoded->rd = getRd(instr);
        decoded->op2 = getOperand2(instr);
    } else if (decoded->type == MUL) {
        decoded->accumulate = toAccumulate(instr);
        decoded->set = isSet(instr);
        decoded->rd = getRd_MUL(instr);
        decoded->rn = getRn_MUL(instr);
        decoded->rs = getRs_MUL(instr);
        decoded->rm = getRm_MUL(instr);
    } else if (decoded->type == TRANSFER) {
        decoded->immediate = isImmediate(instr);
        decoded->p_index = isP_indexing(instr);
        decoded->up = isUp(instr);
        decoded->load = isLoad(instr);
        decoded->rn = getRn(instr);
        decoded->rd = getRd(instr);
        decoded->offset = getOffset_TRANSFER(instr);
    } else {
        decoded->offset = getOffset_BRANCH(instr);
    }
}


//extra functions - for Data Processing execution:
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
    N ? (arm->cpsr_reg |= N_MASK) : (arm->cpsr_reg &= ~(N_MASK));
    Z ? (arm->cpsr_reg |= Z_MASK) : (arm->cpsr_reg &= ~(Z_MASK));
    C ? (arm->cpsr_reg |= C_MASK) : (arm->cpsr_reg &= ~(C_MASK));
    V ? (arm->cpsr_reg |= V_MASK) : (arm->cpsr_reg &= ~(V_MASK));
}
