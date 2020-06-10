#include "define_structures.h"
#include "define_types.h"


Instr *instr_to_bits(Token token, symbol_table, unsigned int address);

Instr *data_proc_to_bits(Token *token, Instr *instruction);

Instr *mul_to_bits(Token *token, Instr *instruction);

Instr *data_transfer_to_bits(Token *token, Instr *instruction, unsigned int address);

Instr *branch_to_bits(Token *token, Instr *instruction, symbol_table);

Instr* special_to_bits(Token * token, Instr* instruction);

