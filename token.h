// Author: Austin Wildgrube <akwwb6@mail.umsl.edu>
// Date: 03/08/2021

#ifndef TOKEN_H
#define TOKEN_H

std::string keywordArray[20] = {
        "begin",
        "end",
        "loop",
        "whole",
        "void",
        "exit",
        "getter",
        "outter",
        "main",
        "if",
        "then",
        "assign",
        "data",
        "proc"
};

std::string tokenID[] {
        "KW_tk",
        "ID_tk",
        "NUM_tk",
        "OP_tk",
        "DEL_tk",
        "EOF_tk"
};

std::string tokenName[] {
        "Keyword",
        "Identifier",
        "Number",
        "Operator",
        "Delimiter",
        "End of File"
};

// Our Token struct is what we print from
struct Token {
    std::string id;
    std::string name;
    std::string userInput;
    int successId = 0;
    int lineNumber = 0;
};

#endif //TOKEN_H
