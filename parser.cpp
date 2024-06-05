#include "Parser.h"
#include <iostream>

ParserException::ParserException(const std::string& message) : message(message) {}

const char* ParserException::what() const noexcept {
    return message.c_str();
}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

std::shared_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_shared<ProgramNode>();
    while (pos < tokens.size() && tokens[pos].type != END_OF_LINE) {
        program->statements.push_back(parseStatement());
    }
    return program;
}


std::shared_ptr<StatementNode> Parser::parseStatement() {
    if (match(COMMAND, "LET")) {
        return parseLetStatement();
    } else if (match(COMMAND, "PRINT")) {
        return parsePrintStatement();
    } else {
        throw ParserException("Unexpected command: " + tokens[pos].value);
    }
}

std::shared_ptr<LetStatementNode> Parser::parseLetStatement() {
    consume(COMMAND, "LET");
    auto letStmt = std::make_shared<LetStatementNode>();
    letStmt->identifier = consume(IDENTIFIER).value;
    consume(OPERATOR, "=");
    letStmt->expression = parseExpression();
    return letStmt;
}

std::shared_ptr<PrintStatementNode> Parser::parsePrintStatement() {
    consume(COMMAND, "PRINT");
    auto printStmt = std::make_shared<PrintStatementNode>();
    printStmt->expression = parseExpression();
    return printStmt;
}

std::shared_ptr<ExpressionNode> Parser::parseExpression() {
    auto left = parsePrimary();
    while (match(OPERATOR)) {
        auto op = consume(OPERATOR).value;
        auto right = parsePrimary();
        auto binaryExpr = std::make_shared<BinaryExpressionNode>();
        binaryExpr->op = op;
        binaryExpr->left = left;
        binaryExpr->right = right;
        left = binaryExpr;
    }
    return left;
}

std::shared_ptr<ExpressionNode> Parser::parsePrimary() {
    if (match(NUMBER)) {
        return std::make_shared<NumberNode>(consume(NUMBER).value);
    } else if (match(IDENTIFIER)) {
        auto identifierToken = consume(IDENTIFIER).value;
        if (match(LPAREN)) {
            auto functionCall = std::make_shared<FunctionCallNode>(identifierToken);
            consume(LPAREN);
            if (!match(RPAREN)) {
                functionCall->arguments.push_back(parseExpression());
                while (match(COMMA)) {
                    consume(COMMA);
                    functionCall->arguments.push_back(parseExpression());
                }
            }
            consume(RPAREN);
            return functionCall;
        }
        return std::make_shared<IdentifierNode>(identifierToken);
    } else if (match(FUNCTION)) {
        auto functionToken = consume(FUNCTION).value;
        auto functionCall = std::make_shared<FunctionCallNode>(functionToken);
        consume(LPAREN);
        functionCall->arguments.push_back(parseExpression());
        while (match(COMMA)) {
            consume(COMMA);
            functionCall->arguments.push_back(parseExpression());
        }
        consume(RPAREN);
        return functionCall;
    } else if (match(LPAREN)) {
        consume(LPAREN);
        auto expr = parseExpression();
        consume(RPAREN);
        return expr;
    } else {
        throw ParserException("Unexpected token: " + tokens[pos].value);
    }
}

bool Parser::match(TokenType type, const std::string& value) {
    if (pos < tokens.size() && tokens[pos].type == type && (value.empty() || tokens[pos].value == value)) {
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& value) {
    if (match(type, value)) {
        return tokens[pos++];
    }
    throw ParserException("Expected token " + tokenTypeName(type) + " but found " + tokens[pos].value);
}

std::string Parser::tokenTypeName(TokenType type) {
    switch (type) {
        case COMMAND: return "COMMAND";
        case IDENTIFIER: return "IDENTIFIER";
        case NUMBER: return "NUMBER";
        case OPERATOR: return "OPERATOR";
        case FUNCTION: return "FUNCTION";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case COMMA: return "COMMA";
        case END_OF_LINE: return "END_OF_LINE";
        default: return "UNKNOWN";
    }
}

void printAST(const std::shared_ptr<ASTNode>& node, int indent) {
    std::string indentStr(indent, ' ');
    if (auto program = std::dynamic_pointer_cast<ProgramNode>(node)) {
        std::cout << indentStr << "ProgramNode" << std::endl;
        for (const auto& stmt : program->statements) {
            printAST(stmt, indent + 2);
        }
    } else if (auto letStmt = std::dynamic_pointer_cast<LetStatementNode>(node)) {
        std::cout << indentStr << "LetStatementNode: " << letStmt->identifier << std::endl;
        printAST(letStmt->expression, indent + 2);
    } else if (auto printStmt = std::dynamic_pointer_cast<PrintStatementNode>(node)) {
        std::cout << indentStr << "PrintStatementNode" << std::endl;
        printAST(printStmt->expression, indent + 2);
    } else if (auto binaryExpr = std::dynamic_pointer_cast<BinaryExpressionNode>(node)) {
        std::cout << indentStr << "BinaryExpressionNode: " << binaryExpr->op << std::endl;
        printAST(binaryExpr->left, indent + 2);
        printAST(binaryExpr->right, indent + 2);
    } else if (auto number = std::dynamic_pointer_cast<NumberNode>(node)) {
        std::cout << indentStr << "NumberNode: " << number->value << std::endl;
    } else if (auto identifier = std::dynamic_pointer_cast<IdentifierNode>(node)) {
        std::cout << indentStr << "IdentifierNode: " << identifier->name << std::endl;
    } else if (auto functionCall = std::dynamic_pointer_cast<FunctionCallNode>(node)) {
        std::cout << indentStr << "FunctionCallNode: " << functionCall->functionName << std::endl;
        for (const auto& arg : functionCall->arguments) {
            printAST(arg, indent + 2);
        }
    }
}
