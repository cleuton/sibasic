#include "Lexer.h"
#include <sstream>
#include "util.h"

LexerException::LexerException(const std::string& message, const std::string& basicLineNumber, const std::string& instruction)
    : message("Line " + basicLineNumber + ": " + message + " - \"" + instruction + "\"") {}

const char* LexerException::what() const noexcept {
    return message.c_str();
}

Lexer::Lexer()
    : commands({{"DIM", true}, {"END", true}, {"LET", true}, {"PRINT", true}, {"GOTO", true}, {"IF", true}}),
      functions({{"EXP", true}, {"ABS", true}, {"LOG", true}, {"SIN", true}, {"COS", true}, {"TAN", true}, {"SQR", true}}),
      operators({{'+', true}, {'-', true}, {'*', true}, {'/', true}, {'^', true}, {'>', true}, {'<', true}, {'=', true}, {'!', true}}) {}



std::vector<Token> Lexer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    this->pos = 0;
    this->length = input.length();
    this->input = input;
    while (pos < length) {
        if (isspace(input[pos])) {
            pos++;
            continue;
        }

        if (pos == 0) {
            std::istringstream lineStream(input);
            std::string basicLineNumber;
            lineStream >> basicLineNumber;
            if (!isNumeric(basicLineNumber)) {
                throw LexerException("Linha sem numero: " + std::string(1, input[pos]), basicLineNumber, input);
            }
            tokens.push_back({LINENO, basicLineNumber});
            pos += basicLineNumber.length();
            continue;
        }

        if (isalpha(input[pos])) {
            std::string word = readWhile([](int c) { return std::isalnum(c); });
            if (commands.count(word)) {
                tokens.push_back({COMMAND, word});
            } else if (functions.count(word)) {
                tokens.push_back({FUNCTION, word});
            } else {
                tokens.push_back({IDENTIFIER, word});
            }
        } else if (isdigit(input[pos]) || input[pos] == '.') {
            tokens.push_back({NUMBER, readWhile([](int c) { return std::isdigit(c) || c == '.'; })});
        } else if (input[pos] == '(') {
            tokens.push_back({LPAREN, "("});
            pos++;
        } else if (input[pos] == ')') {
            tokens.push_back({RPAREN, ")"});
            pos++;
        } else if (input[pos] == '[') {
            tokens.push_back({LCHAVE, "["});
            pos++;
        } else if (input[pos] == ']') {
            tokens.push_back({RCHAVE, "]"});
            pos++;
        } else if (input[pos] == ',') {
            tokens.push_back({COMMA, ","});
            pos++;
        } else if (operators.count(input[pos])) {
            if (input[pos] == '-') {
                // Vamos trocar por "-1 *"
                tokens.push_back({LPAREN, "("});
                tokens.push_back({NUMBER, "1"});
                tokens.push_back({OPERATOR, "-"});
                tokens.push_back({NUMBER, "2"});
                tokens.push_back({RPAREN, ")"});
                tokens.push_back({OPERATOR, "*"});
                pos++;
            } else {
                tokens.push_back({OPERATOR, std::string(1, input[pos])});
                pos++;
            }
        } else {
            throw LexerException("Unexpected character: " + std::string(1, input[pos]), basicLineNumber, input);
        }
    }
    tokens.push_back({END_OF_LINE, ""});
    validateCommand(tokens);
    return tokens;
}

std::string Lexer::readWhile(std::function<bool(int)> condition) {
    std::string result;
    while (pos < length && condition(input[pos])) {
        result += input[pos];
        pos++;
    }
    return result;
}

void Lexer::validateCommand(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        throw LexerException("Empty command", basicLineNumber, input);
    }

    const std::string& command = tokens[1].value;
    if (command == "LET") {
        bool temEsquerdo = false;
        bool temOperador = false;
        bool temDireito = false;
        for (int x=2; x<tokens.size(); x++) {
            if (tokens[x].type == IDENTIFIER) {
                temEsquerdo = true;
            } else if (tokens[x].type == OPERATOR) {
                temOperador = true;
            } else if ((tokens[x].type == IDENTIFIER || tokens[x].type == NUMBER || tokens[x].type == FUNCTION || tokens[x].type == OPERATOR) && temEsquerdo) {
                temDireito = true;
                break;
            }
        }
        if ((!temEsquerdo) || (!temOperador) || (!temDireito)) {
            throw LexerException("Invalid LET statement", basicLineNumber, input);
        }
    } else if (command == "DIM") {
        if (tokens.size() != 5 || tokens[2].type != IDENTIFIER || tokens[3].type != NUMBER) {
            throw LexerException("Invalid DIM statement", basicLineNumber, input);
        }
    } else if (command == "PRINT") {
        int paren_count = 0;
        for (const auto& token : tokens) {
            if (token.type == LPAREN) paren_count++;
            if (token.type == RPAREN) paren_count--;
        }
        if (paren_count != 0) {
            throw LexerException("Mismatched parentheses in PRINT statement", basicLineNumber, input);
        }
        if (tokens.size() < 3) {
            throw LexerException("Invalid PRINT statement", basicLineNumber, input);
        }
    } else if (command == "GOTO") {
        if (tokens.size() != 4 || tokens[2].type != NUMBER) {
            throw LexerException("Invalid GOTO statement", basicLineNumber, input);
        }
    } else if (command == "IF") {
        if (tokens.size() < 7 || (tokens[2].type != IDENTIFIER && tokens[2].type != NUMBER) || tokens[3].type != OPERATOR ||
            (tokens[4].type != IDENTIFIER && tokens[4].type != NUMBER) || tokens[5].value != "THEN" || tokens[6].type != NUMBER) {
            throw LexerException("Invalid IF statement", basicLineNumber, input);
        }
    } else if (command == "END") {
        if (tokens.size() != 3) {
            throw LexerException("Invalid END statement", basicLineNumber, input);
        }
    }
}
