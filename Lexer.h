#ifndef LEXER_H
#define LEXER_H

/*
Copyright 2024 Cleuton Sampaio de Melo Junir

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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