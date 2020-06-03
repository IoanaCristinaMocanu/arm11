#include <stdio.h>
#include "emulator_processor.h"
#include "emulator_processor.h"
#include "decode_helpers.h"
#include <string.h>

#define N_MASK 1 << 31
#define Z_MASK 1 << 30
#define C_MASK 1 << 29
#define V_MASK 1 << 28

#define DATA_PROC_FLAG_MASK 0xe // 1110
#define MUL_FLAG_MASK 0xc // 1100

// --
// -- Instruction execution
// --

// -- Data processing instruction

void data_process(Decoded_Instr* instr, Machine* arm, ProcFunc data_proc_func[14]) {
	uint32_t op1 = arm->general_reg[instr->rn];
	uint8_t code = instr->opcode;
	ProcFunc func = data_proc_func[code];
	uint32_t op2 = decode_offset(instr->op2,instr->imm,arm);
	uint32_t res = func(op1, op2, instr->set, arm);
	instr->offset = instr->op2;
	instr->op2 = op2;
	if(code != 8 && code != 9 && code != 10) {
		arm->general_reg[instr->rd] = res;
	}
	return;
}

// Data processing functions

uint32_t func_and(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 & op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = arm->shifter_carry;
		set_flags(arm,N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

uint32_t func_eor(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 ^op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = arm->shifter_carry;
		set_flags(arm,N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;

}

uint32_t func_sub(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 - op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = op1 >= op2;
		set_flags(arm,N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

uint32_t func_rsb(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op2 - op1;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = op2 >= op1;
		set_flags(arm,N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;

}

uint32_t func_add(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 + op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = (res < op1);
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

uint32_t func_tst(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 & op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = arm->shifter_carry;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return 0;
}

uint32_t func_teq(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 ^op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = arm->shifter_carry;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return 0;
}

uint32_t func_cmp(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 - op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = op1 >= op2;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return 0;
}

uint32_t func_orr(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 | op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = arm->shifter_carry;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

uint32_t func_mov(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 0 : 1;
		uint8_t C = arm->shifter_carry;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

// Functions pointer array

void init_data_proc_func(ProcFunc func_array[14]) {
	func_array[0] = func_and;
	func_array[1] = func_eor;
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

void multiply(Decoded_Instr *instr, Machine *arm) {
	uint32_t result;
	uint32_t rs = arm->general_reg[instr->rs];
	uint32_t rm = arm->general_reg[instr->rm];
	result = rm * rs;
	if (instr->accum) {
		result += arm->general_reg[instr->rn];
	}
	arm->general_reg[instr->rd] = result;
}

//Barrel shifter

uint32_t barrel_shift(uint32_t to_shift,uint8_t ammount,uint8_t type,Machine *arm){
	uint8_t *carry = &(arm->shifter_carry);
	if(ammount == 0) {
		*carry = 0;
		return to_shift;
	}
	switch(type) {
	case 0:   // logical left
		if(ammount >= 32) {
			*carry = 0;
			return 0;
		}
		else {
			*carry = (0x1 << (32 - ammount)) & to_shift;
			return to_shift << ammount;
		}
	case 1:   //logical right
		if(ammount >= 32) {
			*carry = 0;
			return 0;
		}
		else {
			*carry = (0x1 << (ammount - 1)) & to_shift;
			return to_shift >> ammount;
		}
	case 2:   // arithmetic right
		if(ammount >= 32) {
			if(to_shift >> 31) {
				return 0xffffffff;
				*carry = -1;
			}
			else {
				return 0;
				*carry = 0;
			}
		}
		else {
			*carry = (0x1 << (ammount - 1)) & to_shift;
			if(to_shift >> 31) {
				uint32_t bottom = to_shift >> ammount;
				uint32_t top = 0xffffffff << (32 - ammount);
				return top | bottom;
			}
			else {
				return to_shift >> ammount;
			}
		}
	case 3:   // rotate right
		ammount = ammount % 32;
		*carry = (0x1 << (ammount - 1)) & to_shift;
		uint32_t bottom = to_shift >> ammount;
		uint8_t offset = 32 - ammount;
		uint32_t top = (to_shift << offset);
		return top | bottom;
	default:
		return 0;
	}
}

// -- Single data transfer instruction

void data_transfer(Decoded_Instr *instr, Machine *arm){
	uint32_t offset;
	if(!instr->imm) { // opposed to data instr imm
		offset = instr->offset;
	}
	else {
		offset = decode_offset(instr->offset,0,arm);
	}
	if(!instr->up) {
		offset = -offset;
	}

	uint32_t rn;
	if(instr->rn == 15) {
		rn = arm->pc_reg;
	}
	else {
		rn = arm->general_reg[instr->rn];
	}

	if(instr->pre_index) {
		rn += offset;
	}
	else {
		arm->general_reg[instr->rn] += offset;
	}

	if(rn + 4 >= MEMORY_SIZE) {
		printf("Error: Out of bounds memory access at address 0x%08x\n", rn);
		return;
	}

	// assuming a valid adress in memory is provided
	if(instr->load) {
		memcpy(&arm->general_reg[instr->rd],&arm->memory[rn],sizeof(uint32_t));
	}
	else {
		memcpy(&arm->memory[rn],&arm->general_reg[instr->rd],sizeof(uint32_t));
	}

	// TODO: check if RN is PC register !!!
}
// -- Branch Instruction

void branch(Decoded_Instr *instr, Machine *arm){
	int32_t val = instr->sgn_offset;
	val = val << 2;
	if(val & (1 << 23)) {
		val |= (0xff << 24);
	}
	else {
		val &= ~(0xff << 24);
	}
	val = val << 2;
	arm->pc_reg += val;
	arm->branch_executed = true;
}

void execute(Decoded_Instr *instr, Machine *arm, ProcFunc data_proc_func[14]) {
	if(instr->type == HALT) {
		arm->end = true;
		return;
	}

	if(!check_condition(arm,instr)) {
		return;
	}
//	printf("CONDITION PASSED!");

	switch (instr->type) {
	case DATA_PROC:
		data_process(instr, arm, data_proc_func);
		return;
	case MUL:
		multiply(instr, arm);
		return;
	case TRANSFER:
		data_transfer(instr, arm);
		return;
	case BRANCH:
		branch(instr, arm);
		return;
	case NOOP:
		return;
	default:
		return;
	}
}

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
		decoded->rs = get_rs_MUL(instr);
		decoded->rm = get_rm_MUL(instr);
	} else if (decoded->type == TRANSFER) {
		decoded->imm = is_immediate(instr);
		decoded->pre_index = is_pre_index(instr);
		decoded->up = is_up(instr);
		decoded->load = is_load(instr);
		decoded->rn = get_rn(instr);
		decoded->rd = get_rd(instr);
		decoded->offset = get_offset_TRANSFER(instr);
	} else {
		decoded->sgn_offset = get_offset_BRANCH(instr);
	}
}

// extra functions - for Data Processing execution:
// Check if the cond field is satisfied by the CPSR register
bool check_condition(Machine *arm, Decoded_Instr *instruction) {
	uint32_t cond = instruction->cond;
	uint32_t cpsr = arm->cpsr_reg;
	uint32_t get_N = N_MASK & cpsr;
	uint32_t get_Z = Z_MASK & cpsr;
//	uint32_t get_C = C_MASK & cpsr;
	uint32_t get_V = V_MASK & cpsr;

//	printf("CONDITION: %x NZCV:%x %x %x %x",cond,get_N,get_Z,get_C,get_V);

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
void set_flags(Machine *arm, uint8_t N, uint8_t Z, uint8_t C, uint8_t V, uint8_t update_mask) {
	if (update_mask & (1 << 3)) {
		N ? (arm->cpsr_reg |= N_MASK) : (arm->cpsr_reg &= ~(N_MASK));
	}
	if (update_mask & (1 << 2)) {
		Z ? (arm->cpsr_reg |= Z_MASK) : (arm->cpsr_reg &= ~(Z_MASK));
	}
	if (update_mask & (1 << 1)) {
		C ? (arm->cpsr_reg |= C_MASK) : (arm->cpsr_reg &= ~(C_MASK));
	}
	if (update_mask & 1) {
		V ? (arm->cpsr_reg |= V_MASK) : (arm->cpsr_reg &= ~(V_MASK));
	}
}
