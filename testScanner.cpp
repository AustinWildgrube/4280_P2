// Author: Austin Wildgrube <akwwb6@mail.umsl.edu>
// Date: 03/08/2021

#include <string>
#include <cstdio>

#include "testScanner.h"
#include "scanner.h"

using namespace std;

/**
 * This is where we read each individual character and pass it to our scanner. The scanner will do the rest of the work.
 * @param fileName
 */
void TestScanner::testDriver(const char* fileName) {
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