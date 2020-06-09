cmake_minimum_required(VERSION 3.16)
project(arm11_18 C)

set(CMAKE_C_STANDARD 11)

include_directories(src)
include_directories(src/assembler_utils)

add_executable(arm11_18
        src/assembler_utils/array_of_strings.h
        src/assembler_utils/arrays_of_strings.c
        src/assembler_utils/arrays_of_strings.h
        src/assembler_utils/constants.h
        src/assembler_utils/define_types.h
        src/assembler_utils/symbol_table.c
        src/assembler_utils/symbol_table.h
        src/assemble.c
        src/define_types.h
        src/emulate.c
        src/emulator_processor.c
        src/emulator_processor.h
        src/emulator_utils.c
        src/emulator_utils.h)
