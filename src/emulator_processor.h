#ifndef EM_PROC_H
#define EM_PROC_H

#include <stdint.h>
#include <stdbool.h>
#include "define_types.h"

// Execute the decoded instruction 'instr'

void execute(Decoded_Instr *instr, Machine *arm);

// Decode the fetched 'instr' and store it in 'decoded'

void decode(Decoded_Instr *decoded, Instr *instr, Machine *arm);

// TODO: actual bit-wise and calculation functions

#endif
