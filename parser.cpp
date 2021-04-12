// Author: Austin Wildgrube <akwwb6@mail.umsl.edu>
// Date: 04/04/2021

#include <iostream>
#include <cstdio>
#include <utility>

#include "parser.h"
#include "scanner.h"
#include "node.h"

using namespace std;

FILE *file;
Token globalToken;
char character, lookAhead;

/**
 * Main parse function
 * @param fileName
 */
void Parser::parser(const char* fileName) {
    struct Node* root = nullptr;

    // Open our file
    file = fopen(fileName, "r");

    Token fakeToken;
    fakeToken.block = "<program>";
    fakeToken.userInput = "main";
    root = Parser::addStructure(root, fakeToken, nullptr, "");

    // Get our first token
    globalToken = Parser::getNewToken();

    // Start parsing
    Parser::programToken(root);

    // Print our parse tree
    Parser::printPreorder(root);
}

/**
 * Add a node to the parse tree
 * @param node
 * @param token
 * @return
 */
struct Node *Parser::addStructure(struct Node *node, const Token& token, struct Node *originNode,
        string userInput = "") {
    if (!userInput.empty()) {
        node->token.userInput = std::move(userInput);
    }

    // If there is no node then create one
    if (node == nullptr) {
        Node *newNode = new Node(token);

        if (originNode == nullptr) {
            newNode->level = 0;
        } else {
            newNode->level = originNode->level + 1;
        }

        return newNode;
    }

    return node;
}

/**
 * Print the parse tree
 * @param node
 */
void Parser::printPreorder(struct Node* node) {
    // Check to make sure tree isn't empty
    if (node == nullptr)
        return;

    for (int j = 0; j < node->level; j++) {
        cout << "  ";
    }

    // After printing the spaces we then print the leaf info
    cout << node->token.block << " " << node->token.userInput << endl;

    //recursively process children
    Parser::printPreorder(node->childOne);
    Parser::printPreorder(node->childTwo);
    Parser::printPreorder(node->childThree);
    Parser::printPreorder(node->childFour);
}

/**
 * Program token
 * @param node
 * @return
 */
struct Node *Parser::programToken(struct Node* node) {
    if ((globalToken.id == "KW_tk" && globalToken.userInput != "main") || (globalToken.id == "ID_tk")) {
        node->childOne = Parser::varsToken(node->childOne, node);
        Parser::programToken(node->childOne);
    } else if (globalToken.id == "KW_tk" && globalToken.userInput == "main") {
        globalToken = Parser::getNewToken();
        node->childTwo = Parser::blockToken(node->childTwo, node);
    } else {
        cout << "Expected an variable or the main keyword, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Block token
 * @param node
 * @return
 */
struct Node *Parser::blockToken(struct Node* node, struct Node* originNode) {
    if (globalToken.id == "KW_tk" && globalToken.userInput == "begin") {
        globalToken.userInput = "begin end";
        globalToken.block = "<block>";

        node = Parser::addStructure(node, globalToken, originNode);

        globalToken = Parser::getNewToken();
        if ((globalToken.id == "KW_tk" && globalToken.userInput != "main") || (globalToken.id == "ID_tk")) {
            node->childOne = Parser::varsToken(node->childOne, node);

            // Do stats after variables
            node->childTwo = Parser::statsToken(node->childTwo, node);

            if (globalToken.userInput != "end") {
                cout << "Expected the end keyword, found " << globalToken.userInput << endl;
                exit(1);
            }
        } else {
            cout << "Expected a variable, found " << globalToken.userInput << endl;
            exit(1);
        }
    } else {
        cout << "Expected the begin keyword, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Vars token
 * EX: data _MIN := 3;
 * @param node
 * @return
 */
struct Node *Parser::varsToken(struct Node* node, struct Node* originNode) {
    string statement;

    if (globalToken.id == "KW_tk" && globalToken.userInput == "data") {
        statement += globalToken.userInput;
        globalToken = Parser::getNewToken();

        if (globalToken.id == "ID_tk") {
            statement += " ";
            statement += globalToken.userInput;

            globalToken = Parser::getNewToken();

            if (globalToken.id == "OP_tk" && globalToken.userInput == ":=") {
                statement += " ";
                statement += globalToken.userInput;

                globalToken = Parser::getNewToken();

                if (globalToken.id == "NUM_tk") {
                    statement += " ";
                    statement += globalToken.userInput;

                    globalToken = Parser::getNewToken();

                    if (globalToken.id == "DEL_tk" && globalToken.userInput == ";") {
                        statement += globalToken.userInput;

                        globalToken.userInput = statement;
                        globalToken.block = "<vars>";

                        node = Parser::addStructure(node, globalToken, originNode);
                    } else {
                        cout << "Expected a semicolon, found " << globalToken.userInput << endl;
                        exit(1);
                    }
                } else {
                    cout << "Expected an integer, found " << globalToken.userInput << endl;
                    exit(1);
                }
            } else {
                cout << "Expected a := delimiter, found " << globalToken.userInput << endl;
                exit(1);
            }
        } else {
            cout << "Expected an identifier, found " << globalToken.userInput << endl;
            exit(1);
        }
    } else {
        return node;
    }

    statement = "";
    globalToken = Parser::getNewToken();
    if (globalToken.id == "KW_tk" && globalToken.userInput == "data") {
        node->childOne = Parser::varsToken(node->childOne, node);
    }

    return node;
}

/**
 * Expr token
 * @param node
 * @return
 */
struct Node *Parser::exprToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<expr>";

    node = Parser::addStructure(node, fakeToken, originNode);

    node->childOne = Parser::nToken(node->childOne, node);

    if (globalToken.id == "OP_tk" && globalToken.userInput == "-") {
        node = Parser::addStructure(node, fakeToken, originNode, globalToken.userInput);

        globalToken = getNewToken();
        node->childTwo = Parser::exprToken(node->childTwo, node);
    }

    return node;
}

/**
 * N token
 * @param node
 * @return
 */
struct Node *Parser::nToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<N>";
    node = Parser::addStructure(node, fakeToken, originNode);

    node->childOne = Parser::aToken(node->childOne, node);

    if (globalToken.id == "OP_tk" && (globalToken.userInput == "/" || globalToken.userInput == "*")) {
        node = Parser::addStructure(node, fakeToken, originNode, globalToken.userInput);

        globalToken = getNewToken();
        node->childTwo = Parser::nToken(node->childTwo, node);
    }

    return node;
}

/**
 * A token
 * @param node
 * @return
 */
struct Node *Parser::aToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<A>";
    node = Parser::addStructure(node, fakeToken, originNode);

    node->childOne = Parser::mToken(node->childOne, node);

    globalToken = getNewToken();
    if (globalToken.id == "OP_tk" && globalToken.userInput == "+") {
        node = Parser::addStructure(node, fakeToken, originNode, globalToken.userInput);

        globalToken = getNewToken();
        node->childTwo = Parser::aToken(node->childTwo, node);
    }

    return node;
}

/**
 * M token
 * @param node
 * @return
 */
struct Node *Parser::mToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<M>";

    if (globalToken.id == "OP_tk" && globalToken.userInput == "*") {
        fakeToken.userInput = "*";
        node = Parser::addStructure(node, fakeToken, originNode);

        globalToken = getNewToken();
        node->childOne = Parser::mToken(node->childOne, node);
    } else {
        node = Parser::addStructure(node, fakeToken, originNode);
        node->childOne = Parser::rToken(node->childOne, node);
    }

    return node;
}

/**
 * R token
 * @param node
 * @return
 */
struct Node *Parser::rToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<R>";

    if (globalToken.id == "DEL_tk" && globalToken.userInput == "(") {
        fakeToken.userInput = "( )";
        node = Parser::addStructure(node, fakeToken, originNode);

        globalToken = getNewToken();
        node->childOne = Parser::exprToken(node->childOne, node);

        if (globalToken.userInput != ")") {
            cout << "Expected a closing parenthesis, found " << globalToken.userInput << endl;
            exit(1);
        }
    } else if (globalToken.id == "ID_tk" || globalToken.id == "NUM_tk") {
        fakeToken.userInput = globalToken.userInput;
        node = Parser::addStructure(node, fakeToken, originNode);
    } else {
        cout << "Expected an (expression), identifier, or integer found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Stats token
 * @param node
 * @return
 */
struct Node *Parser::statsToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<stats>";

    node = Parser::addStructure(node, fakeToken, originNode);

    node->childOne = Parser::statToken(node->childOne, node);

    globalToken = Parser::getNewToken();
    node->childTwo = Parser::mStatToken(node->childTwo, node);

    return node;
}

/**
 * mStat token
 * @param node
 * @return
 */
struct Node *Parser::mStatToken(struct Node* node, struct Node* originNode) {
    if (globalToken.id == "KW_tk" && globalToken.userInput != "end") {
        Token fakeToken;
        fakeToken.block = "<mStat>";

        node = Parser::addStructure(node, fakeToken, originNode);

        if (globalToken.userInput == "getter") {
            node->childOne = Parser::statToken(node->childOne, node);
        } else if (globalToken.userInput == "outter") {
            node->childOne = Parser::statToken(node->childOne, node);
        } else if (globalToken.userInput == "if") {
            node->childOne = Parser::statToken(node->childOne, node);
        } else if (globalToken.userInput == "loop") {
            node->childOne = Parser::statToken(node->childOne, node);
        } else if (globalToken.userInput == "assign") {
            node->childOne = Parser::statToken(node->childOne, node);
        } else if (globalToken.userInput == "void") {
            node->childOne = Parser::statToken(node->childOne, node);
        } else if (globalToken.userInput == "proc") {
            node->childOne = Parser::statToken(node->childOne, node);
        } else if (globalToken.userInput == "begin") {
            node->childOne = Parser::statToken(node->childOne, node);
        }

        globalToken = Parser::getNewToken();
        node->childTwo = Parser::mStatToken(node->childTwo, node);
    }

    return node;
}

/**
 * Stat token
 * @param node
 * @return
 */
struct Node *Parser::statToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<stat>";
    fakeToken.userInput = ";";

    node = Parser::addStructure(node, fakeToken, originNode);

    if (globalToken.id == "KW_tk" && globalToken.userInput == "getter") {
        node->childOne = Parser::inToken(node->childOne, node);
        globalToken = getNewToken();
    } else if (globalToken.id == "KW_tk" && globalToken.userInput == "outter") {
        node->childOne = Parser::outToken(node->childOne, node);
    } else if (globalToken.id == "KW_tk" && globalToken.userInput == "if") {
        node->childOne = Parser::ifToken(node->childOne, node);
    } else if (globalToken.id == "KW_tk" && globalToken.userInput == "loop") {
        node->childOne = Parser::loopToken(node->childOne, node);
    } else if (globalToken.id == "KW_tk" && globalToken.userInput == "assign") {
        node->childOne = Parser::assignToken(node->childOne, node);
    } else if (globalToken.id == "KW_tk" && globalToken.userInput == "void") {
        node->childOne = Parser::labelToken(node->childOne, node);
        globalToken = getNewToken();
    } else if (globalToken.id == "KW_tk" && globalToken.userInput == "proc") {
        node->childOne = Parser::gotoToken(node->childOne, node);
        globalToken = getNewToken();
    } else if (globalToken.id == "KW_tk" && globalToken.userInput == "begin") {
        node->childOne = Parser::blockToken(node->childOne, node);
    } else {
        cout << "Expected a keyword (in, out, if), found " << globalToken.userInput << endl;
        exit(1);
    }

//    if (globalToken.id != "DEL_tk" && globalToken.userInput != ";") {
//        cout << "Expected a semicolon, found " << globalToken.userInput << endl;
//        exit(1);
//    }

    return node;
}

/**
 * In Token
 * @param node
 * @return
 */
struct Node *Parser::inToken(struct Node* node, struct Node* originNode) {
    string statement = "getter ";

    globalToken = getNewToken();
    globalToken.block = "<in>";

    if (globalToken.id == "ID_tk") {
        statement += globalToken.userInput;
        globalToken.userInput = statement;

        node = Parser::addStructure(node, globalToken, originNode);
    } else {
        cout << "Expected a identifier, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Out Token
 * @param node
 * @return
 */
struct Node *Parser::outToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<out>";
    fakeToken.userInput = "outter";

    node = Parser::addStructure(node, fakeToken, originNode);

    globalToken = getNewToken();
    node->childOne = Parser::exprToken(node->childOne, node);

    return node;
}

/**
 * If Token
 * @param node
 * @return
 */
struct Node *Parser::ifToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<if>";
    fakeToken.userInput = "if [ ] then";

    node = Parser::addStructure(node, fakeToken, originNode);

    globalToken = getNewToken();
    if (globalToken.id == "DEL_tk" && globalToken.userInput == "[") {
        globalToken = getNewToken();
        node->childOne = Parser::exprToken(node->childOne, node);

        node->childTwo = Parser::r0Token(node->childTwo, node);

        globalToken = getNewToken();
        node->childThree = Parser::exprToken(node->childThree, node);

        if (globalToken.id == "DEL_tk" && globalToken.userInput == "]") {
            globalToken = getNewToken();
            if (globalToken.id == "KW_tk" && globalToken.userInput == "then") {
                globalToken = getNewToken();
                node->childFour = Parser::statToken(node->childFour, node);

                globalToken = getNewToken();
                if (globalToken.id != "DEL_tk" && globalToken.userInput != ";") {
                    cout << "Expected a semicolon, found " << globalToken.userInput << endl;
                    exit(1);
                }
            } else {
                cout << "Expected the then keyword, found " << globalToken.userInput << endl;
                exit(1);
            }
        } else {
            cout << "Expected a closing bracket, found " << globalToken.userInput << endl;
            exit(1);
        }
    } else {
        cout << "Expected an open bracket, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Loop Token
 * @param node
 * @return
 */
struct Node *Parser::loopToken(struct Node* node, struct Node* originNode) {
    Token fakeToken;
    fakeToken.block = "<loop>";
    fakeToken.userInput = "loop [ ]";

    node = Parser::addStructure(node, fakeToken, originNode);

    globalToken = getNewToken();
    if (globalToken.id == "DEL_tk" && globalToken.userInput == "[") {
        globalToken = getNewToken();
        node->childOne = Parser::exprToken(node->childOne, node);

        node->childTwo = Parser::r0Token(node->childTwo, node);

        globalToken = getNewToken();
        node->childThree = Parser::exprToken(node->childThree, node);

        if (globalToken.id == "DEL_tk" && globalToken.userInput == "]") {
            globalToken = getNewToken();
            node->childFour = Parser::statToken(node->childFour, node);
        } else {
            cout << "Expected a closing bracket, found " << globalToken.userInput << endl;
            exit(1);
        }
    } else {
        cout << "Expected an open bracket, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Assign Token
 * @param node
 * @return
 */
struct Node *Parser::assignToken(struct Node* node, struct Node* originNode) {
    string statement;

    if (globalToken.id == "KW_tk" && globalToken.userInput == "assign") {
        statement += globalToken.userInput;
        statement += " ";

        globalToken = getNewToken();
        if (globalToken.id == "ID_tk") {
            statement += globalToken.userInput;
            statement += " ";

            globalToken = getNewToken();
            if (globalToken.id == "OP_tk" && globalToken.userInput == ":=") {
                statement += globalToken.userInput;
                globalToken.userInput = statement;
                globalToken.block = "<assign>";

                node = Parser::addStructure(node, globalToken, originNode);

                globalToken = getNewToken();
                node->childOne = Parser::exprToken(node->childOne, node);
            } else {
                cout << "Expected an := operator, found " << globalToken.userInput << endl;
                exit(1);
            }
        } else {
            cout << "Expected an identifier, found " << globalToken.userInput << endl;
            exit(1);
        }
    } else {
        cout << "Expected the keyword assign, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * R0 Token
 * @param node
 * @return
 */
struct Node *Parser::r0Token(struct Node* node, struct Node* originNode) {
    // TODO: Three tokens
    if (globalToken.id == "OP_tk" && (globalToken.userInput == "=>" || globalToken.userInput == "=<"
        || globalToken.userInput == "==" || globalToken.userInput == "%")) {
        globalToken.block = "<R0>";
        node = Parser::addStructure(node, globalToken, originNode);
    } else if (globalToken.id == "DEL_tk" && globalToken.userInput == "[") {
        string statement = globalToken.userInput;
        statement += " ";
        globalToken = Parser::getNewToken();
        if (globalToken.userInput == "==") {
            statement += globalToken.userInput;
            globalToken = Parser::getNewToken();
            if (globalToken.userInput == "]") {
                statement += " ";
                statement += globalToken.userInput;
                globalToken.block = "<R0>";
                globalToken.userInput = statement;
                node = Parser::addStructure(node, globalToken, originNode);
            } else {
                cout << "Expected a closing bracket, found " << globalToken.userInput << endl;
                exit(1);
            }
        } else {
            cout << "Expected ==, found " << globalToken.userInput << endl;
            exit(1);
        }
    } else {
        cout << "Expected an operator or an open bracket, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Label Token
 * @param node
 * @return
 */
struct Node *Parser::labelToken(struct Node* node, struct Node* originNode) {
    string statement = "void ";

    globalToken = getNewToken();
    globalToken.block = "<label>";

    if (globalToken.id == "ID_tk") {
        statement += globalToken.userInput;
        globalToken.userInput = statement;

        node = Parser::addStructure(node, globalToken, originNode);
    } else {
        cout << "Expected a identifier, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Goto Token
 * @param node
 * @return
 */
struct Node *Parser::gotoToken(struct Node* node, struct Node* originNode) {
    string statement = "proc ";

    globalToken = getNewToken();
    globalToken.block = "<goto>";

    if (globalToken.id == "ID_tk") {
        statement += globalToken.userInput;
        globalToken.userInput = statement;

        node = Parser::addStructure(node, globalToken, originNode);
    } else {
        cout << "Expected a identifier, found " << globalToken.userInput << endl;
        exit(1);
    }

    return node;
}

/**
 * Get a new token from our scanner
 * @return
 */
Token Parser::getNewToken() {
    Token returnToken;

    do {
        // Grab our character and look ahead
        character = getc(file);
        lookAhead = getc(file);

        // Call the scanner
        returnToken = Scanner::scan(file, character, lookAhead);
    } while (returnToken.successId == 0);

    return returnToken;
}