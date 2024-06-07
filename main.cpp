#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>
#include <sstream>
#include <fstream>

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

void runProgram(const std::string& input, bool verbose) {
    std::istringstream inputStream(input);
    std::string line;
    auto program = std::make_shared<ProgramNode>();

    while (std::getline(inputStream, line)) {
        Lexer lexer{};
        try {
            std::vector<Token> tokens = lexer.tokenize(line);

            if (verbose) {
                std::cout << "Line " << tokens[0].value << " tokens:" << std::endl;
                printTokens(tokens);
            }

            Parser parser(tokens);
            auto lineProgram = parser.parse();
            for (const auto& statement : lineProgram->statements) {
                program->statements.push_back(statement);
            }

            if (verbose) {
                std::cout << "AST for line " << tokens[0].value << ":" << std::endl;
                printAST(lineProgram);
            }

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

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [-v] <arquivo>" << std::endl;
        return 1;
    }

    bool verbose = false;
    std::string filename;

    if (argc == 3) {
        std::string arg1 = argv[1];
        if (arg1 == "-v") {
            verbose = true;
        } else {
            std::cerr << "Invalid option: " << arg1 << std::endl;
            return 1;
        }
        filename = argv[2];
    } else {
        filename = argv[1];
    }

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

    runProgram(input, verbose);

    return 0;
}
