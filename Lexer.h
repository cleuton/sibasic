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
    std::string numeroDeLinhaBasic;

    std::unordered_map<std::string, bool> comandos;
    std::unordered_map<std::string, bool> funcoes;
    std::unordered_map<char, bool> operadores;

    std::string lerEnquanto(std::function<bool(int)> condicao);
    void validarComando(const std::vector<Token>& tokens);
};

#endif // LEXER_H