#include "emulator_processor.h"
#include "decode_helpers.h"

#define N_MASK 1 << 31
#define Z_MASK 1 << 30
#define C_MASK 1 << 29
#define V_MASK 1 << 28

#define DATA_PROC_FLAG_MASK 0111
#define MUL_FLAG_MASK 1100

// --
// -- Instruction execution
// --

// -- Data processing instruction

void data_process(Decoded_Instr *instr,Machine *arm,ProcFunc data_proc_func[14]){
	uint32_t op1 = arm->general_reg[instr->rn];
	ProcFunc func = data_proc_func[instr->opcode];
	arm->general_reg[instr->rd]=func(op1,instr->op2,instr->set,arm);
	return;
}

// Data processing functions

uint32_t func_and(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;
}
uint32_t func_xor(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;

}
uint32_t func_sub(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;

}
uint32_t func_rsb(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;

}
uint32_t func_add(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;

}
uint32_t func_tst(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;

}
uint32_t func_teq(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;

}
uint32_t func_cmp(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;

}
uint32_t func_orr(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;

}
uint32_t func_mov(uint32_t op1,uint32_t op2,bool flags,Machine *arm){
	return 0;
}

void execute(Decoded_Instr *instr, Machine *arm,ProcFunc data_proc_func[14]){
	switch(instr->type) {
	case HALT:
		arm->end = true;
		return;
	case DATA_PROC:
		data_process(instr,arm,data_proc_func);
	case MUL:
		multiply(instr,arm);
		return;
	case TRANSFER:
		data_transfer(instr,arm);
		return;
	case BRANCH:
		branch(instr,arm);
		return;
	}
}

// Functions pointer array

void init_data_proc_func(ProcFunc func_array[14]){
	func_array[0] = func_and;
	func_array[1] = func_xor;
	func_array[2] = func_sub;
	func_array[3] = func_rsb;
	func_array[4] = func_add;
	func_array[8] = func_tst;
	func_array[9] = func_teq;
	func_array[10] = func_cmp;
	func_array[12] = func_orr;
	func_array[13] = func_mov;
}

// -- Multiply instruction

void multiply(Decoded_Instr *instr,Machine *arm){
	uint32_t result;
	if(instr->accum) {
		result = instr->rm_val * instr->rs_val + general_reg[instr->rn];
	}
}

// -- Single data transfer instruction

void data_transfer(Decoded_Instr *instr,Machine *arm);

// -- Branch Instruction

void branch(Decoded_Instr *instr,Machine *arm);

// --
// -- Instructions decoding
// --

// Decode the fetched 'instr' and store it in 'decoded'

void decode(Decoded_Instr *decoded, Instr *instr, Machine *arm) {
	decoded->type = get_instr_type(instr);
	decoded->exists = true;
	decoded->cond = get_cond(instr);

	if (decoded->type == DATA_PROC) {
		decoded->imm = is_immediate(instr);
		decoded->opcode = get_opcode(instr);
		decoded->set = is_set(instr);
		decoded->rn = get_rn(instr);
		decoded->rd = get_rd(instr);
		decoded->op2 = get_operand2(instr);
	} else if (decoded->type == MUL) {
		decoded->accum = to_accumulate(instr);
		decoded->set = is_set(instr);
		decoded->rd = get_rd_MUL(instr);
		decoded->rn = get_rn_MUL(instr);
		decoded->rs_val = get_rs_MUL(instr); // TODO: also process
		decoded->rm_val = get_rm_MUL(instr); // TODO: also process
	} else if (decoded->type == TRANSFER) {
		decoded->imm = is_immediate(instr);
		decoded->pre_index = is_pre_index(instr);
		decoded->up = is_up(instr);
		decoded->load = is_load(instr);
		decoded->rn = get_rn(instr);
		decoded->rd = get_rn(instr);
		decoded->offset = get_offset_TRANSFER(instr);
	} else {
		decoded->offset = get_offset_BRANCH(instr);
	}
}

// extra functions - for Data Processing execution:
// Check if the cond field is satisfied by the CPSR register
bool check_condition(Machine *arm, Instr *instruction) {
	uint32_t cond = get_cond(instruction);
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

// set CPSR flags
void set_flags(Machine *arm, uint8_t N, uint8_t Z, uint8_t C, uint8_t V,uint8_t update_mask) {
	if(update_mask & 1 << 3) {
		N ? (arm->cpsr_reg |= N_MASK) : (arm->cpsr_reg &= ~(N_MASK));
	}
	if(update_mask & 1 << 2) {
		Z ? (arm->cpsr_reg |= Z_MASK) : (arm->cpsr_reg &= ~(Z_MASK));
	}
	if(update_mask & 1 << 1) {
		C ? (arm->cpsr_reg |= C_MASK) : (arm->cpsr_reg &= ~(C_MASK));
	}
	if(update_mask & 1) {
		V ? (arm->cpsr_reg |= V_MASK) : (arm->cpsr_reg &= ~(V_MASK));
	}
}
