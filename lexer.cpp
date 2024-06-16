#include "Lexer.h"
#include <sstream>
#include "util.h"

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

LexerException::LexerException(const std::string& message, const std::string& basicLineNumber, const std::string& instruction)
    : message("Line " + basicLineNumber + ": " + message + " - \"" + instruction + "\"") {}

const char* LexerException::what() const noexcept {
    return message.c_str();
}

Lexer::Lexer()
    : comandos({{"DIM", true}, {"END", true}, {"LET", true}, {"PRINT", true}, {"GOTO", true}, {"IF", true}, {"INPUT", true},
                {"DRAW", true}, {"PLOT", true}, {"LINE", true}, {"RECTANGLE", true}}),
      funcoes({{"EXP", true}, {"ABS", true}, {"LOG", true}, {"SIN", true}, {"COS", true}, {"TAN", true}, {"SQR", true}, {"RND", true}}),
      operadores({{'+', true}, {'-', true}, {'*', true}, {'/', true}, {'^', true}, {'>', true}, {'<', true}, {'=', true}, {'!', true}}) {}



std::vector<Token> Lexer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    this->pos = 0;
    this->length = input.length();
    this->input = input;
    bool trocarUnaryMinus = false;
    while (pos < length) {
        if (isspace(input[pos])) {
            pos++;
            continue;
        }

        if (pos == 0) {
            trocarUnaryMinus = false;
            std::istringstream lineStream(input);
            std::string basicLineNumber;
            lineStream >> basicLineNumber;
            if (!isNumeric(basicLineNumber)) {
                throw LexerException("Linha sem numero: " + std::string(1, input[pos]), basicLineNumber, input);
            }
            tokens.push_back({NUMERO_LINHA, basicLineNumber});
            pos += basicLineNumber.length();
            continue;
        }

        if (input[pos] == '\"') {
            // inicio de literal
            trocarUnaryMinus = false;
            pos++;
            std::string literal = lerEnquanto([](int c) { return c != '\"'; });
            if (pos<input.length()) {
                pos++;
            }
            tokens.push_back({LITERAL_TEXTO, literal});
        } else if (isalpha(input[pos])) {
            trocarUnaryMinus = false;
            std::string word = lerEnquanto([](int c) { return std::isalnum(c); });
            if (comandos.count(word)) {
                tokens.push_back({COMANDO, word});
            } else if (funcoes.count(word)) {
                tokens.push_back({FUNCAO, word});
            } else {
                tokens.push_back({IDENTIFICADOR, word});
            }
        } else if (isdigit(input[pos]) || input[pos] == '.') {
            trocarUnaryMinus = false;
            tokens.push_back({NUMERO, lerEnquanto([](int c) { return std::isdigit(c) || c == '.'; })});
        } else if (input[pos] == '(') {
            trocarUnaryMinus = true;
            tokens.push_back({PARENTESIS_ESQUERDO, "("});
            pos++;
        } else if (input[pos] == ')') {
            trocarUnaryMinus = false;
            tokens.push_back({PARENTESIS_DIREITO, ")"});
            pos++;
        } else if (input[pos] == '[') {
            trocarUnaryMinus = false;
            tokens.push_back({CHAVE_ESQUERDA, "["});
            pos++;
        } else if (input[pos] == ']') {
            trocarUnaryMinus = false;
            tokens.push_back({CHAVE_DIREITA, "]"});
            pos++;
        } else if (input[pos] == ',') {
            trocarUnaryMinus = false;
            tokens.push_back({VIRGULA, ","});
            pos++;
        } else if (input[pos] == '"') {
            trocarUnaryMinus = false;
            tokens.push_back({ASPAS_DUPLAS, "\""});
            pos++;
        } else if (operadores.count(input[pos])) {
            if (trocarUnaryMinus && input[pos] == '-') {
                // Vamos trocar por "-1 *"
                tokens.push_back({PARENTESIS_ESQUERDO, "("});
                tokens.push_back({NUMERO, "1"});
                tokens.push_back({OPERADOR, "-"});
                tokens.push_back({NUMERO, "2"});
                tokens.push_back({PARENTESIS_DIREITO, ")"});
                tokens.push_back({OPERADOR, "*"});
                trocarUnaryMinus = false;
                pos++;
            } else {
                trocarUnaryMinus = true;
                tokens.push_back({OPERADOR, std::string(1, input[pos])});
                pos++;
            }
        } else {
            throw LexerException("Caractere inesperado: " + std::string(1, input[pos]), numeroDeLinhaBasic, input);
        }
    }
    tokens.push_back({FIM_DE_LINHA, ""});
    validarComando(tokens);
    return tokens;
}

std::string Lexer::lerEnquanto(std::function<bool(int)> condicao) {
    std::string result;
    while (pos < length && condicao(input[pos])) {
        result += input[pos];
        pos++;
    }
    return result;
}

void Lexer::validarComando(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        throw LexerException("Comando vazio", numeroDeLinhaBasic, input);
    }

    const std::string& command = tokens[1].value;
    if (command == "LET") {
        /* Aqui podemos validar o LET */
    } else if (command == "DIM") {
        if (tokens.size() != 5 || tokens[2].type != IDENTIFICADOR || tokens[3].type != NUMERO) {
            throw LexerException("Comando DIM inválido", numeroDeLinhaBasic, input);
        }
    } else if (command == "PRINT") {
        if (tokens.size() < 3) {
            throw LexerException("Comando PRINT inválido", numeroDeLinhaBasic, input);
        }
    } else if (command == "GOTO") {
        if (tokens.size() != 4 || tokens[2].type != NUMERO) {
            throw LexerException("Comando GOTO inválido", numeroDeLinhaBasic, input);
        }
    } else if (command == "IF") {
        /* Aqui podemos validar o IF*/
    } else if (command == "END") {
        if (tokens.size() != 3) {
            throw LexerException("Comando END invalido", numeroDeLinhaBasic, input);
        }
    } else if (command == "DRAW") {
        if (tokens.size() < 4) {
            throw LexerException("Comando DRAW inválido 1", numeroDeLinhaBasic, input);
        }
        if (tokens[2].type != IDENTIFICADOR) {
            throw LexerException("Comando DRAW inválido 2", numeroDeLinhaBasic, input);
        }
        if (tokens[2].value != "START" && tokens[2].value != "FINISH") {
            throw LexerException("Comando DRAW inválido 3", numeroDeLinhaBasic, input);
        }
        if (tokens[2].value == "START") {
            if (tokens.size() != 7) {
                throw LexerException("Comando DRAW inválido 4", numeroDeLinhaBasic, input);
            }
            if ((tokens[3].type != IDENTIFICADOR && tokens[3].type != NUMERO)) {
                throw LexerException("Comando DRAW inválido 5", numeroDeLinhaBasic, input);
            }
        }

    } else if (command == "PLOT" ) {
        if (tokens.size() < 10) {
            throw LexerException("Comando PLOT inválido 1", numeroDeLinhaBasic, input);
        }
    } else if (command == "LINE") {
        if (tokens.size() < 12) {
            throw LexerException("Comando LINE inválido 1", numeroDeLinhaBasic, input);
        }
    } else if (command == "RECTANGLE") {
        if (tokens.size() < 12) {
            throw LexerException("Comando RECTANGLE inválido 1", numeroDeLinhaBasic, input);
        }
    }

}
