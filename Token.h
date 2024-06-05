#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType {
    COMMAND, IDENTIFIER, NUMBER, OPERATOR, FUNCTION, LPAREN, RPAREN, COMMA, END_OF_LINE, LINENO
};

struct Token {
    TokenType type;
    std::string value;
};

#endif // TOKEN_H