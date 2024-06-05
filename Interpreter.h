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
    void executeStatement(const std::shared_ptr<ASTNode>& statement);
    double evaluateExpression(const std::shared_ptr<ASTNode>& expression);

private:
    // Função para converter graus em radianos
    double degreesToRadians(double degrees);
    std::unordered_map<std::string, double> variables;
    // Função para processar chamadas de função
    double processFunction(const std::string& functionName, double argument);
};
#endif //SIBASIC_INTERPRETER_H
