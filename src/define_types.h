//
// Created by bianca on 5/28/20.
//

#ifndef ARM11_18_DEFINE_TYPES_H
#define ARM11_18_DEFINE_TYPES_H
#define MEMORY_SIZE 64000
#define GENERAL_REGISTERS_NUM 13

struct Machine {
    uint8_t memory[MEMORY_SIZE];
    uint32_t general_reg[GENERAL_REGISTERS_NUM];
    uint32_t cpsr_reg;
    uint32_t pc_reg;
    bool end;
};
typedef struct Machine Machine;

enum cond {
    eq = 0b0000,
    ne = 0b0001,
    ge = 0b1010,
    lt = 0b1011,
    gt = 0b1100,
    le = 0b1101,
    al = 0b1110
};

enum type{
    DATA_PROC,
    MUL,
    TRANSFER,
    BRANCH,
    NOOP = 0xffffffff
};
struct Decoded_Instr {
    bool exists;
    enum cond cond;
    enum type type; // 4 main types of instr + noop
    uint8_t rn;
    uint8_t rd;
    uint32_t offset;

    // data processing
    uint32_t op2;
    bool set;

    //multiply
    uint32_t rm_val;
    uint32_t rs_val;

    // single data transfer
    bool imm;
    bool pre;
    bool load;
    bool up;
};
typedef struct Decoded_Instr Decoded_Instr;

struct Instr {
    bool exists;
    uint32_t bits;
};
typedef struct Instr Instr;

#endif //ARM11_18_DEFINE_TYPES_H
