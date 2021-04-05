// Author: Austin Wildgrube <akwwb6@mail.umsl.edu>
// Date: 04/04/2021

#include <iostream>
#include <cstdio>
#include <iomanip>

#include "parser.h"
#include "scanner.h"

using namespace std;

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
        Token returnToken = Scanner::scan(file, character, lookAhead);
        cout << returnToken.lineNumber << setw(20);
        cout << returnToken.name << setw(20);
        cout << returnToken.id << setw(20);
        cout << returnToken.successId << setw(20);
        cout << returnToken.userInput << endl;
    } while (character != EOF);
}