#include <stdlib.h>
#include <stdio.h>

/*
 * 1. verify the number of passed arguments
 * 2. initialize the source and output filenames
 * 3. load the assembly instructions
 * 4. generate symbol table
 * 5. binary encoding + save the files
 * 6. free the memory + exit
 */

int main(int argc, char **argv) {

    //1. verify the number of passed arguments
    if (argc != 3) {
        printf(stderr, "The number of provided arguments is not correct");
        return EXIT_FAILURE;
    }

    //2. initialize the source and output filenames
    char *filename_source = argv[1];
    char *filename_output = argv[2];

    //3. load the assembly instructions
    //4. generate symbol table
    int input_lines;
    return EXIT_SUCCESS;
}
