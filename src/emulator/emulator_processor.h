#ifndef EM_PROC_H
#define EM_PROC_H

#include <stdint.h>
#include <stdbool.h>
#include "define_structures.h"

// Execute the decoded instruction 'instr'

void execute(Decoded_Instr *instr, Machine *arm,ProcFunc data_proc_func[14]);

// Data processing functions

void init_data_proc_func(ProcFunc func[14]);

// Decode the fetched 'instr' and store it in 'decoded'

void decode(Decoded_Instr *decoded, Instr *instr, Machine *arm);

// TODO: actual bit-wise and calculation functions
//check if the Cond field is satisfied by the CPSR register
bool check_condition(Machine *arm, Instr *instruction);

//set CPSR flags
void set_flags(Machine *arm, uint8_t N, uint8_t Z, uint8_t C, uint8_t V);
#endif
