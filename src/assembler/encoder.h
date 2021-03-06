#include "define_types.h"
#include "label_table.h"

void instr_to_bits(Token *token, label_dict *dict, uint16_t address,uint16_t last_address,uint16_t* ldr_count,uint32_t *ldr_imm_values, uint32_t *binary);

void data_proc_to_bits(Token *token, uint32_t *binary);

void mul_to_bits(Token *token, uint32_t *binary);

void data_block_data_transfer_to_bits(Token *token, uint32_t *binary);

void data_transfer_to_bits(Token *token, uint16_t address,uint16_t last_address,uint16_t *ldr_count,uint32_t *ldr_imm_values, uint32_t *binary);

void branch_to_bits(Token *token, uint32_t *binary, label_dict *dict);

void special_to_bits(Token *token, uint32_t *binary, label_dict *dict);

void address_to_bits(Address address, uint32_t *binary);

uint32_t convert_op2(uint32_t op2);
