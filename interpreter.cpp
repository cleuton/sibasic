#include "Interpreter.h"
#include "Parser.h"
#include <cmath>
#include <stdexcept>
#include <memory>
#include <iostream>

Interpreter::Interpreter() {
    // Inicialize variáveis com um map vazio
    variables = std::unordered_map<std::string, double>();
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
    for (const auto& statement : program->statements) {
        executeStatement(statement);
    }
}

void Interpreter::executeStatement(const std::shared_ptr<ASTNode>& statement) {
    if (auto letStmt = std::dynamic_pointer_cast<LetStatementNode>(statement)) {
        double value = evaluateExpression(letStmt->expression);
        variables[letStmt->identifier] = value;
    } else if (auto printStmt = std::dynamic_pointer_cast<PrintStatementNode>(statement)) {
        double value = evaluateExpression(printStmt->expression);
        std::cout << value << std::endl;
    } else {
        throw std::runtime_error("Unexpected statement type");
    }
}

double Interpreter::evaluateExpression(const std::shared_ptr<ASTNode>& expression) {
    if (auto numberNode = std::dynamic_pointer_cast<NumberNode>(expression)) {
        return std::stod(numberNode->value);
    } else if (auto identifierNode = std::dynamic_pointer_cast<IdentifierNode>(expression)) {
        if (variables.find(identifierNode->name) != variables.end()) {
            return variables[identifierNode->name];
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
