#ifndef ARM11_18_DEFINE_TYPES_H
#define ARM11_18_DEFINE_TYPES_H

#include <stdint.h>
#include <stdbool.h>

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
    SHIFT_ROR = 0x3,
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
    uint8_t format;
    shift_t type;
    union {
        uint8_t regist;
        uint8_t expression;
    } args;
} Shift;

/*
 * encoded operand2
 */
typedef struct {
    bool immediate;
    union {
        //uint8_t expression;
        int expression;
        struct {
            uint8_t rm;
            Shift shift;
        } shifted_register;
    } Register;
} Operand2;

/*
 * encoded address
 */

typedef struct {
    uint8_t format;
    union {
        uint8_t expression;
        struct {
            uint8_t format;
            bool pre_post_index;
            uint8_t rn;
            union {
                uint8_t expression;
                struct {
                    uint8_t pm;
                    uint8_t rm;
                    Shift shift;
                } Shift;
            } Offset;
        } Register;
    } Expression;
} Address;

/*
 * the information from an instruction is encoded with a token
 */

typedef struct {
    uint16_t address;
    mnemonic_t opcode;
//    unsigned char num_args;
    bool flag;
    condition_t condition;
    union {
        struct {
            uint8_t rd;
            uint8_t rn;
            Operand2 op2;
        } data_processing;
        struct {
            uint8_t rd;
            uint8_t rm;
            uint8_t rs;
            uint8_t rn;
        } multiply;
        struct {
            uint8_t rd;
            Address address;
        } transfer;
        struct {
            char *expression;
        } branch;
    } Content;
} Token;


#endif
