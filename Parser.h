#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include <memory>
#include <vector>
#include <string>
#include <exception>

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

class ProgramNode : public ASTNode {
public:
    std::vector<ASTNodePtr> statements;
};

class StatementNode : public ASTNode {
public:
    virtual ~StatementNode() = default;
};

class LetStatementNode : public StatementNode {
public:
    std::string identifier;
    ASTNodePtr expression;
};

class PrintStatementNode : public StatementNode {
public:
    ASTNodePtr expression;
};

class ExpressionNode : public ASTNode {
public:
    virtual ~ExpressionNode() = default;
};

class FunctionCallNode : public ExpressionNode {
public:
    std::string functionName;
    std::vector<ASTNodePtr> arguments;

    explicit FunctionCallNode(const std::string& functionName) : functionName(functionName) {}
};



class BinaryExpressionNode : public ExpressionNode {
public:
    std::string op;
    ASTNodePtr left;
    ASTNodePtr right;
};

class NumberNode : public ExpressionNode {
public:
    std::string value;
    explicit NumberNode(const std::string& value) : value(value) {}
};

class IdentifierNode : public ExpressionNode {
public:
    std::string name;
    explicit IdentifierNode(const std::string& name) : name(name) {}
};

class ParserException : public std::exception {
public:
    explicit ParserException(const std::string& message);

    const char* what() const noexcept override;

private:
    std::string message;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::shared_ptr<ProgramNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    std::shared_ptr<StatementNode> parseStatement();
    std::shared_ptr<LetStatementNode> parseLetStatement();
    std::shared_ptr<PrintStatementNode> parsePrintStatement();
    std::shared_ptr<ExpressionNode> parseExpression();
    std::shared_ptr<ExpressionNode> parsePrimary();

    bool match(TokenType type, const std::string& value = "");
    Token consume(TokenType type, const std::string& value = "");

    std::string tokenTypeName(TokenType type);
};

void

 printAST(const std::shared_ptr<ASTNode>& node, int indent = 0);

#endif // PARSER_H