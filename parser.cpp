// Author: Austin Wildgrube <akwwb6@mail.umsl.edu>
// Date: 04/04/2021

#include <cstdio>
#include "parser.h"

#include "scanner.h"


void Parser::parser(const char* fileName) {
    FILE *file;
    char character, lookAhead;

    // Open our file
    file = fopen(fileName, "r");

    do {
        // Grab our character and look ahead
        character = getc(file);
        lookAhead = getc(file);

        // Call the scanner
        Scanner::scan(file, character, lookAhead);
    } while (character != EOF);
}