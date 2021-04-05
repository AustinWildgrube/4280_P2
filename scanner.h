// Author: Austin Wildgrube <akwwb6@mail.umsl.edu>
// Date: 03/08/2021

#ifndef SCANNER_H
#define SCANNER_H

#include <string>

class Scanner {
public:
    static void scan(FILE *, char, char);
//        static void scan(const std::string&);
private:
    static int getColumn(char);
    static int searchTokens(int, int);
    static bool checkKeywords(const std::string&);
    static void getPrintStatement(int, const std::string&, int);
    static void getErrorStatement(const std::string&, int);
};

#endif //SCANNER_H