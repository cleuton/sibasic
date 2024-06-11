#include "Interpreter.h"
#include "Parser.h"
#include "util.h"
#include <cmath>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <sstream>
#include <strings.h>

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

Interpreter::Interpreter() {
    // Inicialize variáveis com um map vazio
    variables = std::unordered_map<std::string, std::vector<double>>();
}


double Interpreter::grausParaRadianos(double degrees) {
    return degrees * M_PI / 180.0;
}

double Interpreter::processarFuncao(const std::string& nomeDaFuncao, double argumento) {
    if (nomeDaFuncao == "SIN") {
        return std::sin(grausParaRadianos(argumento));
    } else if (nomeDaFuncao == "COS") {
        return std::cos(grausParaRadianos(argumento));
    } else if (nomeDaFuncao == "TAN") {
        return std::tan(grausParaRadianos(argumento));
    } else if (nomeDaFuncao == "LOG") {
        return std::log(argumento);
    } else if (nomeDaFuncao == "EXP") {
        return std::exp(argumento);
    } else if (nomeDaFuncao == "SQR") {
        return std::sqrt(argumento);
    } else if (nomeDaFuncao == "ABS") {
        return std::abs(argumento);
    }
    throw std::runtime_error("Função não suportada: " + nomeDaFuncao);
}

void Interpreter::executar(const std::shared_ptr<NoDePrograma>& programa) {
    int index = 0;
    while (index < programa->comandos.size()) { // Enquanto o índice for menor que o tamanho do vetor
        const auto& statement = programa->comandos[index];
        int newIndex = executarComando(statement, programa);
        if (newIndex>=0) {
            // Foi um GOTO ou um IF
            index = newIndex;
            continue;
        } else if (newIndex == -2) {
            // Encontrou um comando END
            break;
        }
        ++index; // Incrementa o índice para avançar para o próximo elemento
    }
}

int Interpreter::executarComando(const std::shared_ptr<NoDaAST>& comando, const std::shared_ptr<NoDePrograma>& programa) {
    if (auto letStmt = std::dynamic_pointer_cast<NoDoComandoLET>(comando)) {
        double value = avaliarExpressao(letStmt->expressao);
        if (letStmt->posicao.empty()) {
            // Não deveria ser um vetor...
            if (vetores.find(letStmt->identificador) != vetores.end()) {
                std::ostringstream oss;
                oss << "Vetor deve ser sempre indexado: " << letStmt->identificador;
                throw std::runtime_error(oss.str());
            }
            variables[letStmt->identificador] = std::vector<double>(1, value);
        } else {
            // Deveria ser um vetor...
            if (vetores.find(letStmt->identificador) == vetores.end()) {
                std::ostringstream oss;
                oss << "Variavel nao e um vetor: " << letStmt->identificador;
                throw std::runtime_error(oss.str());
            }
            int posicao = getPosicao(letStmt->identificador, letStmt->posicao);
            variables[letStmt->identificador].at(posicao) = value;
        }
    } else if (auto printStmt = std::dynamic_pointer_cast<NoDoComandoPRINT>(comando)) {
        if (printStmt->printLiteral) {
            std::cout << printStmt->literal << std::endl;
        } else {
            double value = avaliarExpressao(printStmt->expressao);
            std::cout << value << std::endl;
        }
    } else if (auto gotoStmt = std::dynamic_pointer_cast<NoDoComandoGOTO>(comando)) {
        int index = 0;
        while (index < programa->comandos.size()) {
            const auto& statement = std::dynamic_pointer_cast<NoDeComando>(programa->comandos[index]);
            if (statement->numeroLinha == gotoStmt->numeroLinhaDesvio) {
                return index;
            }
            ++index;
        }
        throw std::runtime_error("Numero de linha inexistente");
    } else if (auto dimStmt = std::dynamic_pointer_cast<NoDoComandoDIM>(comando)) {
        if (variables.find(dimStmt->nomeVariavel) != variables.end()) {
            std::ostringstream oss;
            oss << "Já existe variável com esse nome: " << dimStmt->nomeVariavel;
            throw std::runtime_error(oss.str());
        }
        vetores[dimStmt->nomeVariavel] = dimStmt->numeroOcorrencias;
        variables[dimStmt->nomeVariavel] = std::vector<double>(dimStmt->numeroOcorrencias, 0.0);
    } else if (auto endStmt = std::dynamic_pointer_cast<NoDoComandoEND>(comando)) {
        std::cout << "Comando END" << std::endl;
        return -2; // Terminar o programa
    } else if (auto ifStmt = std::dynamic_pointer_cast<NoDoComandoIF>(comando)) {
        double operando1 = avaliarExpressao(ifStmt->operando1);
        double operando2 = avaliarExpressao(ifStmt->operando2);
        double resultado = operando1 - operando2;
        bool trueFalse = false;
        if (ifStmt->operadorLogico == "=") {
            if (resultado == 0) {
                trueFalse = true;
            }
        } else if (ifStmt->operadorLogico == ">") {
            if (resultado > 0) {
                trueFalse = true;
            }
        } else {
            if (resultado < 0) {
                trueFalse = true;
            }
        }
        if (trueFalse) {
            // vai desviar para a linha THEN
            int index = 0;
            while (index < programa->comandos.size()) {
                const auto& statement = std::dynamic_pointer_cast<NoDeComando>(programa->comandos[index]);
                if (statement->numeroLinha == ifStmt->numeroLinha) {
                    return index;
                }
                ++index;
            }
        }
        return -1;
    } else if (auto inputStmt = std::dynamic_pointer_cast<NoDoComandoINPUT>(comando)) {
        double valor;
        std::cout << "# ";
        std::cin >> valor;
        variables[inputStmt->identificador] = std::vector<double>(1, valor);
    } else {
        throw std::runtime_error("Tipo de comando inexperado");
    }
    return -1;
}

int Interpreter::getPosicao(const std::string &variavel, const std::string &indexador) {
    int valor = 0;
    if (isNumeric(indexador)) {
        // É uma posição. vamos ver se ela existe
        valor = std::stoi(indexador);
    } else {
        // É uma variável. Vamos ver se ela existe e pegar seu valor
        if (variables.find(variavel) == variables.end()) {
            std::ostringstream oss;
            oss << "Variavel indexadora nao existe: " << variavel << " >> " << indexador;
            throw std::runtime_error(oss.str());
        }
        if (vetores.find(indexador) != vetores.end()) {
            std::ostringstream oss;
            oss << "Variavel indexadora nao pode ser um vetor: " << variavel << " >> " << indexador;
            throw std::runtime_error(oss.str());
        }
        valor = variables[indexador].at(0);
    }
    if (valor <= 0 || valor > vetores[variavel]) {
        std::ostringstream oss;
        oss << "Posicao invalida para o vetor: " << variavel << " >> " << indexador;
        throw std::runtime_error(oss.str());
    }
    return valor - 1; // No C++ os vetores são zero based.
}


double Interpreter::avaliarExpressao(const std::shared_ptr<NoDaAST>& expressao) {
    if (auto numberNode = std::dynamic_pointer_cast<NoDeNumero>(expressao)) {
        return std::stod(numberNode->value);
    } else if (auto identifierNode = std::dynamic_pointer_cast<NoDeIdentificador>(expressao)) {
        if (variables.find(identifierNode->name) != variables.end()) {
            if (vetores.find(identifierNode->name) != vetores.end()) {
                if (identifierNode->posicao.empty()) {
                    std::ostringstream oss;
                    oss << "Variavel deveria ser indexada pois e um vetor: " << identifierNode->name;
                    throw std::runtime_error(oss.str());
                } else {
                    int posicao = getPosicao(identifierNode->name, identifierNode->posicao);
                    return variables[identifierNode->name].at(posicao);
                }
            } else {
                return variables[identifierNode->name].at(0);
            }
        } else {
            throw std::runtime_error("Variável não declarada: " + identifierNode->name);
        }
    } else if (auto binaryExpr = std::dynamic_pointer_cast<NoDeExpressaoBinaria>(expressao)) {
        double left = avaliarExpressao(binaryExpr->left);
        double right = avaliarExpressao(binaryExpr->right);
        if (binaryExpr->op == "+") {
            return left + right;
        } else if (binaryExpr->op == "-") {
            return left - right;
        } else if (binaryExpr->op == "*") {
            return left * right;
        } else if (binaryExpr->op == "/") {
            return left / right;
        } else if (binaryExpr->op == "^") {
            return std::pow(left, right);
        }
    } else if (auto functionCall = std::dynamic_pointer_cast<NoDeFuncao>(expressao)) {
        double argument = avaliarExpressao(functionCall->argumentos[0]);
        return processarFuncao(functionCall->nomeDaFuncao, argument);
    }
    throw std::runtime_error("Tipo de expressão inesperado");
}
