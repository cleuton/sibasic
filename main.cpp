#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

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

const std::string VERSAO = "0.0.4";

void executarPrograma(const std::string basicScriptName, const std::string& input, bool verbose) {
    std::istringstream inputStream(input);
    std::string linha;
    auto programa = std::make_shared<NoDePrograma>();

    if (verbose) {
        std::cout << "SiBasic v " << VERSAO << std::endl;
    }
    bool jaTemDrawStart = false;
    while (std::getline(inputStream, linha)) {
        Lexer lexer{};
        try {
            if (linha[0] == '*') {
                // É uma linha de comentário. Vamos pular
                continue;
            }
            transform(linha.begin(), linha.end(), linha.begin(), ::toupper);
            std::vector<Token> tokens = lexer.tokenize(linha);

            Parser parser(tokens, jaTemDrawStart);

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
            jaTemDrawStart = parser.jaTemDrawStart;
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
        Interpreter interpreter(basicScriptName);
        interpreter.executar(programa);
    } catch (const std::runtime_error& e) {
        std::cerr << "Erro de interpreter: " << e.what() << std::endl;
    }
}

std::string getScriptName(const std::string filePath) {
    std::filesystem::path pathObj(filePath);
    // Obtendo o nome do arquivo
    return pathObj.filename().string();
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

    std::string basicScriptName = getScriptName(filename);
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Falha ao abrir arquivo: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

    executarPrograma(basicScriptName,input, verbose);

    return 0;
}
