#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>
#include <sstream>

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::string type;
        switch (token.type) {
            case LINENO: type = "NUMERO DA LINHA"; break;
            case COMMAND: type = "COMMAND"; break;
            case IDENTIFIER: type = "IDENTIFIER"; break;
            case NUMBER: type = "NUMBER"; break;
            case OPERATOR: type = "OPERATOR"; break;
            case FUNCTION: type = "FUNCTION"; break;
            case LPAREN: type = "LPAREN"; break;
            case RPAREN: type = "RPAREN"; break;
            case LCHAVE: type = "LCHAVE"; break;
            case RCHAVE: type = "RCHAVE"; break;
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
        Lexer lexer{};
        try {
            std::vector<Token> tokens = lexer.tokenize(line);
            std::cout << "Line " << tokens[0].value << " tokens:" << std::endl;
            printTokens(tokens);

            Parser parser(tokens);
            auto lineProgram = parser.parse();
            for (const auto& statement : lineProgram->statements) {
                program->statements.push_back(statement);
            }

            std::cout << "AST for line " << tokens[0].value << ":" << std::endl;
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
/*
10 DIM V 5
20 LET X = 1
30 LET V[X] = X + 50
40 PRINT V[1]
 */
int main() {
    std::vector<std::string> tests = {
        /*"10 LET A = 5\n20 LET B=(SIN(A) + 3) ^2\n30 GOTO 50\n40 PRINT 999\n50 PRINT B\n"
        "10 DIM V 5\n20 LET X = 1\n30 LET V[X] = X + 50\n40 PRINT V[1]\n"*/
        "1 DIM T 3\n5 LET T[3] = 5\n15 LET T[2] = (SIN(T[3]) + 3) ^2\n20 PRINT T[2]\n"
    };

    for (const auto& test : tests) {
        std::cout << "Testing input:\n" << test << std::endl;
        runTest(test);
    }

    return 0;
}
