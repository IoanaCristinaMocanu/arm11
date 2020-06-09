#ifndef ARM11_18_DEFINE_TYPES_H
#define ARM11_18_DEFINE_TYPES_H

#include <stdint.h>

typedef uint16_t address_t;
typedef uint32_t instr;

/*
 * enum to represent the mnemonic types
 */
typedef enum {
    ADD,
    SUB,
    RSB,
    AND,
    EOR,
    ORR,
    MOV,
    TST,
    TEQ,
    CMP,
    MUL,
    MLA,
    LDR,
    STR,
    BEQ,
    BNE,
    BGE,
    BLT,
    BGT,
    BLE,
    B,
    LSL,
    ANDEQ
} mnemonic_t;

/*
 * enum to represent the shift types
 */
typedef enum {
    SHIFT_LSL = 0x0,
    SHIFT_LSR = 0x1,
    SHIFT_ASR = 0x2,
    SHIFT_ROR = 0X3,
    NO_SHIFT
} shift_t;

/*
 * enum to represent the conditions (the values are the encodings)
 */
typedef enum {
    EQ = 0x0,
    NE = 0x1,
    GE = 0xa,
    LT = 0xb,
    GT = 0xc,
    LE = 0xd,
    AL = 0xe
} condition_t;

/*
 * encoded shift
 */
typedef struct {
    int format;
    shift_t type;
    union {
        unsigned int regist;
        int expression;
    };
} Shift;

/*
 * encoded operand2
 */
typedef struct {
    int immediate;
    union {
        int expression;
        struct {
            int rm;
            Shift shift;
        } shifted_register;
    };
} Operand2;

/*
 * encoded address
 */

typedef struct {
    int format;
    union {
        int expression;
        struct {
            int format;
            int pre_post_index;
            int rn;
            union {
                int expression;
                struct {
                    int pm;
                    unsigned int rm;
                    Shift shift;
                } Shift;
            };
        } Register;
    };
} Address;

/*
 * the information from an instruction is encoded with a token
 */

typedef struct {
    unsigned int address;
    mnemonic_t opcode;
    unsigned char num_args;
    int flag;
    condition_t condition;
    union {
        struct {
            unsigned int rd;
            unsigned int rn;
            Operand2 op2;
        } data_processing;
        struct {
            unsigned int rd;
            unsigned int rm;
            unsigned int rs;
            unsigned int rn;
        } multiply;
        struct {
            unsigned int rd;
            Address address;
        } transfer;
        struct {
            char *expression;
        } branch;
    };
} token;


#endif
