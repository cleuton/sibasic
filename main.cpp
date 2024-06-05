#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>
#include <sstream>

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::string type;
        switch (token.type) {
            case COMMAND: type = "COMMAND"; break;
            case IDENTIFIER: type = "IDENTIFIER"; break;
            case NUMBER: type = "NUMBER"; break;
            case OPERATOR: type = "OPERATOR"; break;
            case FUNCTION: type = "FUNCTION"; break;
            case LPAREN: type = "LPAREN"; break;
            case RPAREN: type = "RPAREN"; break;
            case COMMA: type = "COMMA"; break;
            case END_OF_LINE: type = "END_OF_LINE"; break;
        }
        std::cout << type << ": " << token.value << std::endl;
    }
}

void runTest(const std::string& input) {
    std::istringstream inputStream(input);
    std::string line;
    auto program = std::make_shared<ProgramNode>();

    while (std::getline(inputStream, line)) {
        std::istringstream lineStream(line);
        std::string basicLineNumber;
        lineStream >> basicLineNumber;
        std::string restOfLine;
        std::getline(lineStream, restOfLine);
        restOfLine = line.substr(basicLineNumber.length() + 1); // Retirando o número da linha e o espaço
        try {
            Lexer lexer(restOfLine, basicLineNumber);
            std::vector<Token> tokens = lexer.tokenize();
            std::cout << "Line " << basicLineNumber << " tokens:" << std::endl;
            printTokens(tokens);

            Parser parser(tokens);
            auto lineProgram = parser.parse();
            for (const auto& statement : lineProgram->statements) {
                program->statements.push_back(statement);
            }

            std::cout << "AST for line " << basicLineNumber << ":" << std::endl;
            printAST(lineProgram);

        } catch (const LexerException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        } catch (const ParserException& e) {
            std::cerr << "Parser Error: " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }

    try {
        Interpreter interpreter;
        interpreter.execute(program);
    } catch (const std::runtime_error& e) {
        std::cerr << "Interpreter Error: " << e.what() << std::endl;
    }
}

int main() {
    std::vector<std::string> tests = {
        "10 LET A = 5\n20 LET B=(SIN(A) + 3) ^2\n30 PRINT B\n"
    };

    for (const auto& test : tests) {
        std::cout << "Testing input:\n" << test << std::endl;
        runTest(test);
    }

    return 0;
}
