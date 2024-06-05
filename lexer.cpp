#include "Lexer.h"

LexerException::LexerException(const std::string& message, const std::string& basicLineNumber, const std::string& instruction)
    : message("Line " + basicLineNumber + ": " + message + " - \"" + instruction + "\"") {}

const char* LexerException::what() const noexcept {
    return message.c_str();
}

Lexer::Lexer(const std::string& input, const std::string& basicLineNumber)
    : input(input), pos(0), length(input.length()), basicLineNumber(basicLineNumber),
      commands({{"DIM", true}, {"END", true}, {"LET", true}, {"PRINT", true}, {"GOTO", true}, {"IF", true}}),
      functions({{"EXP", true}, {"ABS", true}, {"LOG", true}, {"SIN", true}, {"COS", true}, {"TAN", true}, {"SQR", true}}),
      operators({{'+', true}, {'-', true}, {'*', true}, {'/', true}, {'^', true}, {'>', true}, {'<', true}, {'=', true}, {'!', true}}) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < length) {
        if (isspace(input[pos])) {
            pos++;
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
        } else if (isdigit(input[pos])) {
            tokens.push_back({NUMBER, readWhile([](int c) { return std::isdigit(c); })});
        } else if (input[pos] == '(') {
            tokens.push_back({LPAREN, "("});
            pos++;
        } else if (input[pos] == ')') {
            tokens.push_back({RPAREN, ")"});
            pos++;
        } else if (input[pos] == ',') {
            tokens.push_back({COMMA, ","});
            pos++;
        } else if (operators.count(input[pos])) {
            tokens.push_back({OPERATOR, std::string(1, input[pos])});
            pos++;
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

    const std::string& command = tokens[0].value;
    if (command == "LET") {
        if (tokens.size() < 4 || tokens[1].type != IDENTIFIER || tokens[2].type != OPERATOR || tokens[2].value != "=") {
            if (!(tokens.size() >= 6 && tokens[1].type == IDENTIFIER && tokens[2].type == LPAREN && tokens[3].type == NUMBER && tokens[4].type == RPAREN && tokens[5].type == OPERATOR && tokens[5].value == "=")) {
                throw LexerException("Invalid LET statement", basicLineNumber, input);
            }
        } else if (tokens.size() == 4 && tokens[3].type == END_OF_LINE) {
            throw LexerException("Invalid LET statement, missing value", basicLineNumber, input);
        } else {
            for (size_t i = 3; i < tokens.size(); ++i) {
                if (tokens[i].type == OPERATOR && i == tokens.size() - 2) {
                    throw LexerException("Invalid LET statement, missing value after operator", basicLineNumber, input);
                }
            }
        }
    } else if (command == "DIM") {
        if (tokens.size() != 4 || tokens[1].type != IDENTIFIER || tokens[2].type != NUMBER) {
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
        if (tokens.size() < 2) {
            throw LexerException("Invalid PRINT statement", basicLineNumber, input);
        }
    } else if (command == "GOTO") {
        if (tokens.size() != 3 || tokens[1].type != NUMBER) {
            throw LexerException("Invalid GOTO statement", basicLineNumber, input);
        }
    } else if (command == "IF") {
        if (tokens.size() < 6 || (tokens[1].type != IDENTIFIER && tokens[1].type != NUMBER) || tokens[2].type != OPERATOR ||
            (tokens[3].type != IDENTIFIER && tokens[3].type != NUMBER) || tokens[4].value != "THEN" || tokens[5].type != NUMBER) {
            throw LexerException("Invalid IF statement", basicLineNumber, input);
        }
    } else if (command == "END") {
        if (tokens.size() != 2) {
            throw LexerException("Invalid END statement", basicLineNumber, input);
        }
    }
}
