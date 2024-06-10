#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>
#include <sstream>
#include <fstream>

void executarPrograma(const std::string& input, bool verbose) {
    std::istringstream inputStream(input);
    std::string linha;
    auto programa = std::make_shared<NoDePrograma>();

    while (std::getline(inputStream, linha)) {
        Lexer lexer{};
        try {
            if (linha[0] == '*') {
                // É uma linha de comentário. Vamos pular
                continue;
            }
            transform(linha.begin(), linha.end(), linha.begin(), ::toupper);
            std::vector<Token> tokens = lexer.tokenize(linha);

            Parser parser(tokens);

            if (verbose) {
                std::cout << "Fonte: " << linha << std::endl;
                std::cout << "Linha " << tokens[0].value << " tokens:" << std::endl;
                for (const auto& token : tokens) {
                    std::string type = parser.tokenTypeName(token.type);
                    std::cout << type << ": " << token.value << std::endl;
                }
            }

            auto lineProgram = parser.parse();
            for (const auto& statement : lineProgram->comandos) {
                programa->comandos.push_back(statement);
            }

            if (verbose) {
                std::cout << "AST para a linha line " << tokens[0].value << ":" << std::endl;
                mostrarAST(lineProgram);
            }

        } catch (const LexerException& e) {
            std::cerr << "Erro de lexer: " << e.what() << std::endl;
        } catch (const ParserException& e) {
            std::cerr << "Erro de parser: " << e.what() << std::endl;
        }
        if (verbose) {
            std::cout << std::endl;
        }
    }

    try {
        Interpreter interpreter;
        interpreter.executar(programa);
    } catch (const std::runtime_error& e) {
        std::cerr << "Erro de interpreter: " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Uso: " << argv[0] << " [-v] <arquivo>" << std::endl;
        return 1;
    }

    bool verbose = false;
    std::string filename;

    if (argc == 3) {
        std::string arg1 = argv[1];
        if (arg1 == "-v") {
            verbose = true;
        } else {
            std::cerr << "Opção inválida: " << arg1 << std::endl;
            return 1;
        }
        filename = argv[2];
    } else {
        filename = argv[1];
    }

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Falha ao abrir arquivo: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

    executarPrograma(input, verbose);

    return 0;
}
