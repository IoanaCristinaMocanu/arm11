#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "emulator_processor.h"

#define TRUE 420

int main(int argc, char **argv) {

	// Argument check and file read
	//
	FILE *input;

	if (argc != 2) {
		perror("Invalid argument number");
		exit(EXIT_FAILURE);
	}
	else {
		if((input = fopen(argv[1],"rb")) == NULL) {
			perror("File could not be found");
			exit(EXIT_FAILURE);
		}
	}

	// -- Declaration and initialization of memory and registers storage
	// --

	Machine arm;

	memset(arm.memory, 0, MEMORY_SIZE * sizeof(uint8_t));
	memset(arm.general_reg, 0, GENERAL_REGISTERS_NUM * sizeof(uint32_t));

	arm.cpsr_reg = 0;
	arm.pc_reg = 0;
	arm.end = false;

	// -- Store input onto memory
	// -- byte-by-byte and then divide count by 4

	int instr_count = 0;

	while(!feof(input)) {
		if(instr_count == MEMORY_SIZE) {
			perror("Instructions exceed memory size");
			exit(EXIT_FAILURE);
		}
		fread(&arm.memory[instr_count],1,1,input);
		++instr_count;
	}

	instr_count = ceil(instr_count / 4);

	// -- Start the pipeline
	// -- initialise all 3 stages to NULL

	Instr fetched_instr;
	fetched_instr.exists = false;
	fetched_instr.instr = 0;
	Decoded_Instr decoded_instr;

	while(!arm.end) {
		if(decoded_instr.exists) {
			execute(&decoded_instr,&arm); // <- execute previously decoded instruction
		}

		if(fetched_instr.exists) {
			decode(&decoded_instr,&fetched_instr,&arm); // <- decode previously fetched instruction
		}

		memcpy(&fetched_instr.instr,&arm.memory[arm.pc_reg],4); // fetch from memory acc to PC
		fetched_instr.exists = true;

		if(fetched_instr.exists) {
			// -- if fetched instruction is halt
			// -- execute last instruction and set exit
			if(fetched_instr.instr == 0) {
				execute(&decoded_instr,&arm);
				arm.end = true;
				arm.pc_reg += 4;
			}
			printf("%x\n",fetched_instr.instr);
		}
		arm.pc_reg += 4;
	}

	// -- Print the final machine state
	// --

	printf("Registers:\n");
	for(int i = 0; i < GENERAL_REGISTERS_NUM; i++) {
		printf("$%-2d : %10d (%#010x)\n",i,arm.general_reg[i],arm.general_reg[i]);
	}
	printf("PC  : %*d (%#010x)\n",10,arm.pc_reg,arm.pc_reg);
	printf("CPSR: %*d (%#010x)\n",10,arm.cpsr_reg,arm.cpsr_reg);

	int val;
	printf("Non-zero memory:\n");
	for(int i = 0; i < MEMORY_SIZE / 4; i++) {
		memcpy(&val,&arm.memory[4*i],4);
		if(val) {
			printf("%#010x : %#010x\n",4*i,val);
		}
	}

	return EXIT_SUCCESS;
}
