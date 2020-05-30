#include <stdio.h>
#include "emulator_processor.h"
#include "emulator_processor.h"
#include "decode_helpers.h"
#include <string.h>

#define N_MASK 1 << 31
#define Z_MASK 1 << 30
#define C_MASK 1 << 29
#define V_MASK 1 << 28

#define DATA_PROC_FLAG_MASK 0111
#define MUL_FLAG_MASK 1100

//Data processing

#define IMM_MASK 0xff
#define ROTATE_MASK (0xf << 8)
#define SHIFT_MASK (0xff << 4)
#define RM_MASK 0xf


// Barrel shifter options

#define LOGICAL_LEFT 0
#define LOGICAL_RIGHT 1
#define ARITHM_RIGHT 2
#define ROTATE_RIGHT 3

// --
// -- Instruction execution
// --

// -- Data processing instruction

void data_process(Decoded_Instr* instr, Machine* arm, ProcFunc data_proc_func[14]) {
	uint32_t op1 = arm->general_reg[instr->rn];
	uint8_t code = instr->opcode;
	ProcFunc func = data_proc_func[code];
	uint32_t op2;

	if(instr->imm) {
		// immediate value
		op2 = instr->op2 & IMM_MASK;
		uint8_t rotate = instr->op2 & ROTATE_MASK;
		op2 = barrel_shift(op2,2*rotate,ROTATE_RIGHT,arm);
	}
	else {
		// shifter register value
		uint8_t shift = instr->op2 & SHIFT_MASK;
		uint8_t rm = instr->op2 & SHIFT_MASK;
		uint32_t val = arm->general_reg[rm];

		if(shift & 1) {
			// constant ammount
			uint8_t amm = (0xf1 << 3) & shift;
			uint8_t type = (0x3 << 1) & shift;
			op2 = barrel_shift(val,amm,type,arm);
		}
		else {
			// ammount specifies by a register
			uint8_t rs = (0xf << 4) & shift;
			uint8_t type = (0x3 << 1) & shift;
			uint8_t amm = arm->general_reg[rs] & 0xff;
			op2 = barrel_shift(val,amm,type,arm);
		}
	}

	uint32_t res = func(op1, op2, instr->set, arm);
	if(code != 8 && code != 9 && code != 10) {
		int pos = instr->rd;
		printf("POS: %x %x\n",pos,res);
		memcpy(&arm->general_reg[instr->rd + 0],&res,sizeof(uint32_t));
		// !!!!!!
		// does only work for +1 or +2 or -1 or whatever but never just instr->rd !!!!!!!!!!!!!!!
		// !!!!!!
	}
	return;
}

// Data processing functions

uint32_t func_and(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 & op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = arm->shifter_carry;
		set_flags(arm,N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

uint32_t func_eor(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 ^op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = arm->shifter_carry;
		set_flags(arm,N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;

}

uint32_t func_sub(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 - op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = 0;
		set_flags(arm,N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

uint32_t func_rsb(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op2 - op1;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = 0;
		set_flags(arm,N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;

}

// TODO: check overflow!!
uint32_t func_add(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 + op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = 0;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

uint32_t func_tst(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 & op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = arm->shifter_carry;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return 0;
}

uint32_t func_teq(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 ^op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = arm->shifter_carry;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return 0;
}

uint32_t func_cmp(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 - op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = 0;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return 0;
}

uint32_t func_orr(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op1 | op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
		uint8_t C = arm->shifter_carry;
		set_flags(arm, N, Z, C, 0,DATA_PROC_FLAG_MASK);
	}
	return res;
}

uint32_t func_mov(uint32_t op1, uint32_t op2, bool flags, Machine *arm) {
	uint32_t res = op2;
	if (flags) {
		uint8_t N = (res & N_MASK) ? 1 : 0;
		uint8_t Z = (res) ? 1 : 0;
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
	if (instr->accum) {
		result = instr->rm_val * instr->rs_val + arm->general_reg[instr->rn];
		arm->general_reg[instr->rd] = result;
	}
	else {
		result = instr->rm_val * instr->rs_val;
		arm->general_reg[instr->rd] = result;
	}
}

//Barrel shifter

uint32_t barrel_shift(uint32_t to_shift,uint8_t ammount,uint8_t type,Machine *arm){
	uint8_t *carry = &(arm->shifter_carry);
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
		uint32_t top = (to_shift << offset) >> offset;
		return top | bottom;
	default:
		return 0;
	}
}

// -- Single data transfer instruction

void data_transfer(Decoded_Instr *instr, Machine *arm){
	int y;
}

// -- Branch Instruction

void branch(Decoded_Instr *instr, Machine *arm){
	int y = 0;
}

void execute(Decoded_Instr *instr, Machine *arm, ProcFunc data_proc_func[14]) {
	switch (instr->type) {
	case HALT:
		arm->end = true;
		return;
	case DATA_PROC:
		data_process(instr, arm, data_proc_func);
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
		printf("Opcode: %x\n",decoded->opcode);
		decoded->set = is_set(instr);
		decoded->rn = get_rn(instr);
		decoded->rd = get_rd(instr);
		decoded->op2 = get_operand2(instr);
	} else if (decoded->type == MUL) {
		decoded->accum = to_accumulate(instr);
		decoded->set = is_set(instr);
		decoded->rd = get_rd_MUL(instr);
		decoded->rn = get_rn_MUL(instr);
		decoded->rs_val = get_rs_MUL(instr);     // TODO: also process
		decoded->rm_val = get_rm_MUL(instr);     // TODO: also process
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
void set_flags(Machine *arm, uint8_t N, uint8_t Z, uint8_t C, uint8_t V, uint8_t update_mask) {
	if (update_mask & 1 << 3) {
		N ? (arm->cpsr_reg |= N_MASK) : (arm->cpsr_reg &= ~(N_MASK));
	}
	if (update_mask & 1 << 2) {
		Z ? (arm->cpsr_reg |= Z_MASK) : (arm->cpsr_reg &= ~(Z_MASK));
	}
	if (update_mask & 1 << 1) {
		C ? (arm->cpsr_reg |= C_MASK) : (arm->cpsr_reg &= ~(C_MASK));
	}
	if (update_mask & 1) {
		V ? (arm->cpsr_reg |= V_MASK) : (arm->cpsr_reg &= ~(V_MASK));
	}
}
