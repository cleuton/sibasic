#ifndef SIBASIC_INTERPRETER_H
#define SIBASIC_INTERPRETER_H

#include "Parser.h"
#include <cmath>
#include <stdexcept>
#include <memory>
#include <unordered_map>

class Interpreter {
public:
    Interpreter();
    void execute(const std::shared_ptr<ProgramNode>& program);
    int executeStatement(const std::shared_ptr<ASTNode>& statement, const std::shared_ptr<ProgramNode>& program);
    double evaluateExpression(const std::shared_ptr<ASTNode>& expression);

private:
    // Função para converter graus em radianos
    double degreesToRadians(double degrees);
    std::unordered_map<std::string, std::vector<double>> variables;
    std::unordered_map<std::string, int> vetores;
    // Função para processar chamadas de função
    double processFunction(const std::string& functionName, double argument);
    int getPosicao(const std::string& varivavel, const std::string& indexador);
};
#endif //SIBASIC_INTERPRETER_H
