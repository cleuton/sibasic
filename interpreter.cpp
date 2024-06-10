#include "Interpreter.h"
#include "Parser.h"
#include "util.h"
#include <cmath>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <sstream>
#include <strings.h>

Interpreter::Interpreter() {
    // Inicialize variáveis com um map vazio
    variables = std::unordered_map<std::string, std::vector<double>>();
}


double Interpreter::degreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double Interpreter::processFunction(const std::string& functionName, double argument) {
    if (functionName == "SIN") {
        return std::sin(degreesToRadians(argument));
    } else if (functionName == "COS") {
        return std::cos(degreesToRadians(argument));
    } else if (functionName == "TAN") {
        return std::tan(degreesToRadians(argument));
    } else if (functionName == "LOG") {
        return std::log(argument);
    } else if (functionName == "EXP") {
        return std::exp(argument);
    } else if (functionName == "SQR") {
        return std::sqrt(argument);
    } else if (functionName == "ABS") {
        return std::abs(argument);
    }
    throw std::runtime_error("Unsupported function: " + functionName);
}

void Interpreter::execute(const std::shared_ptr<ProgramNode>& program) {
    int index = 0;
    while (index < program->statements.size()) { // Enquanto o índice for menor que o tamanho do vetor
        const auto& statement = program->statements[index];
        int newIndex = executeStatement(statement, program);
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

int Interpreter::executeStatement(const std::shared_ptr<ASTNode>& statement, const std::shared_ptr<ProgramNode>& program) {
    if (auto letStmt = std::dynamic_pointer_cast<LetStatementNode>(statement)) {
        double value = evaluateExpression(letStmt->expression);
        if (letStmt->posicao.empty()) {
            // Não deveria ser um vetor...
            if (vetores.find(letStmt->identifier) != vetores.end()) {
                std::ostringstream oss;
                oss << "Vetor deve ser sempre indexado: " << letStmt->identifier;
                throw std::runtime_error(oss.str());
            }
            variables[letStmt->identifier] = std::vector<double>(1, value);
        } else {
            // Deveria ser um vetor...
            if (vetores.find(letStmt->identifier) == vetores.end()) {
                std::ostringstream oss;
                oss << "Variavel nao e um vetor: " << letStmt->identifier;
                throw std::runtime_error(oss.str());
            }
            int posicao = getPosicao(letStmt->identifier, letStmt->posicao);
            variables[letStmt->identifier].at(posicao) = value;
        }
    } else if (auto printStmt = std::dynamic_pointer_cast<PrintStatementNode>(statement)) {
        if (printStmt->printLiteral) {
            std::cout << printStmt->literal << std::endl;
        } else {
            double value = evaluateExpression(printStmt->expression);
            std::cout << value << std::endl;
        }
    } else if (auto gotoStmt = std::dynamic_pointer_cast<GotoStatementNode>(statement)) {
        int index = 0;
        while (index < program->statements.size()) {
            const auto& statement = std::dynamic_pointer_cast<StatementNode>(program->statements[index]);
            if (statement->numeroLinha == gotoStmt->numeroLinhaDesvio) {
                return index;
            }
            ++index;
        }
        throw std::runtime_error("Numero de linha inexistente");
    } else if (auto dimStmt = std::dynamic_pointer_cast<DimStatementNode>(statement)) {
        if (variables.find(dimStmt->nomeVariavel) != variables.end()) {
            std::ostringstream oss;
            oss << "Já existe variável com esse nome: " << dimStmt->nomeVariavel;
            throw std::runtime_error(oss.str());
        }
        vetores[dimStmt->nomeVariavel] = dimStmt->numeroOcorrencias;
        variables[dimStmt->nomeVariavel] = std::vector<double>(dimStmt->numeroOcorrencias, 0.0);
    } else if (auto endStmt = std::dynamic_pointer_cast<EndStatementNode>(statement)) {
        std::cout << "Comando END" << std::endl;
        return -2; // Terminar o programa
    } else if (auto ifStmt = std::dynamic_pointer_cast<IfStatementNode>(statement)) {
        double operando1 = evaluateExpression(ifStmt->operando1);
        double operando2 = evaluateExpression(ifStmt->operando2);
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
            while (index < program->statements.size()) {
                const auto& statement = std::dynamic_pointer_cast<StatementNode>(program->statements[index]);
                if (statement->numeroLinha == ifStmt->numeroLinha) {
                    return index;
                }
                ++index;
            }
        }
        return -1; // Condition was false
    } else {
        throw std::runtime_error("Unexpected statement type");
    }
    return -1; // Continua a iterar no próximo comando
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


double Interpreter::evaluateExpression(const std::shared_ptr<ASTNode>& expression) {
    if (auto numberNode = std::dynamic_pointer_cast<NumberNode>(expression)) {
        return std::stod(numberNode->value);
    } else if (auto identifierNode = std::dynamic_pointer_cast<IdentifierNode>(expression)) {
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
            throw std::runtime_error("Undefined variable: " + identifierNode->name);
        }
    } else if (auto binaryExpr = std::dynamic_pointer_cast<BinaryExpressionNode>(expression)) {
        double left = evaluateExpression(binaryExpr->left);
        double right = evaluateExpression(binaryExpr->right);
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
    } else if (auto functionCall = std::dynamic_pointer_cast<FunctionCallNode>(expression)) {
        double argument = evaluateExpression(functionCall->arguments[0]);
        return processFunction(functionCall->functionName, argument);
    }
    throw std::runtime_error("Unexpected expression type");
}
