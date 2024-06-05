#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <exception>

class LexerException : public std::exception {
public:
    LexerException(const std::string& message, const std::string& basicLineNumber, const std::string& instruction);

    const char* what() const noexcept override;

private:
    std::string message;
};

class Lexer {
public:
    Lexer();

    std::vector<Token> tokenize(const std::string& input);

private:
    std::string input;
    size_t pos;
    size_t length;
    std::string basicLineNumber;

    std::unordered_map<std::string, bool> commands;
    std::unordered_map<std::string, bool> functions;
    std::unordered_map<char, bool> operators;

    std::string readWhile(std::function<bool(int)> condition);
    void validateCommand(const std::vector<Token>& tokens);
};

#endif // LEXER_H