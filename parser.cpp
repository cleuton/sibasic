#include "Parser.h"
#include <iostream>

ParserException::ParserException(const std::string& message) : message(message) {}

const char* ParserException::what() const noexcept {
    return message.c_str();
}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

std::shared_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_shared<ProgramNode>();
    std::string numeroLinha = "";
    while (pos < tokens.size() && tokens[pos].type != END_OF_LINE) {
        if (tokens[pos].type == LINENO) {
            numeroLinha = tokens[pos++].value;
            continue;
        }
        std::shared_ptr<StatementNode> nodePointer = parseStatement();
        nodePointer->numeroLinha = numeroLinha;
        program->statements.push_back(nodePointer);
    }
    return program;
}


std::shared_ptr<StatementNode> Parser::parseStatement() {
    if (match(COMMAND, "LET")) {
        return parseLetStatement();
    } else if (match(COMMAND, "PRINT")) {
        return parsePrintStatement();
    } else if (match(COMMAND, "GOTO")) {
        return parseGotoStatement();
    } else if (match(COMMAND, "DIM")) {
        return parseDimStatement();
    } else if (match(COMMAND, "END")) {
        return parseEndStatement();
    } else if (match(COMMAND, "IF")) {
        return parseIfStatement();
    } else {
        throw ParserException("Unexpected command: " + tokens[pos].value);
    }
}

std::shared_ptr<LetStatementNode> Parser::parseLetStatement() {
    consume(COMMAND, "LET");
    auto letStmt = std::make_shared<LetStatementNode>();
    letStmt->identifier = consume(IDENTIFIER).value().value;
    auto token = consume(LCHAVE, "", false);
    if (token) {
        // É uma variável indexada. Pode ter um INDENTIFIER ou um NUMBER e um RCHAVE
        auto resultado = consume(NUMBER, "",false);
        if (!resultado.has_value()) {
            // é uma variável indexando.
            auto resultado2 = consume(IDENTIFIER);
            if (resultado2.has_value()) {
                letStmt->posicao = resultado2.value().value;
            }
        } else {
            letStmt->posicao = resultado.value().value;
        }
        consume(RCHAVE);
    }
    consume(OPERATOR, "=");
    letStmt->expression = parseExpression();
    return letStmt;
}

std::shared_ptr<DimStatementNode> Parser::parseDimStatement() {
    consume(COMMAND, "DIM");
    auto dimStmt = std::make_shared<DimStatementNode>();
    dimStmt->nomeVariavel = consume(IDENTIFIER).value().value;
    dimStmt->numeroOcorrencias = std::stoi(consume(NUMBER).value().value);
    return dimStmt;
}

std::shared_ptr<PrintStatementNode> Parser::parsePrintStatement() {
    consume(COMMAND, "PRINT");
    auto printStmt = std::make_shared<PrintStatementNode>();
    printStmt->expression = parseExpression();
    return printStmt;
}

std::shared_ptr<GotoStatementNode> Parser::parseGotoStatement() {
    consume(COMMAND, "GOTO");
    auto gotoStmt = std::make_shared<GotoStatementNode>();
    gotoStmt->numeroLinhaDesvio = consume(NUMBER).value().value;
    return gotoStmt;
}

std::shared_ptr<EndStatementNode> Parser::parseEndStatement() {
    consume(COMMAND, "END");
    auto endStmt = std::make_shared<EndStatementNode>();
    return endStmt;
}

std::shared_ptr<IfStatementNode> Parser::parseIfStatement() {
    consume(COMMAND, "IF");
    auto ifStmt = std::make_shared<IfStatementNode>();
    ifStmt->operando1 = parsePrimary();
    auto retorno = consume(OPERATOR, "=", false);
    if (!retorno.has_value()) {
        retorno = consume(OPERATOR, ">", false);
        if (!retorno.has_value()) {
            retorno = consume(OPERATOR, "<", false);
            if (!retorno.has_value()) {
                throw ParserException("IF sem operador lógico");
            }
        }
    }
    ifStmt->operadorLogico = retorno.value().value;
    ifStmt->operando2 = parsePrimary();
    consume(IDENTIFIER, "THEN");
    ifStmt->numeroLinha = consume(NUMBER).value().value;
    return ifStmt;
}

/*
std::shared_ptr<ExpressionNode> Parser::parseExpression() {
    auto left = parsePrimary();
    while (match(OPERATOR)) {
        auto op = consume(OPERATOR).value().value;
        auto right = parsePrimary();
        auto binaryExpr = std::make_shared<BinaryExpressionNode>();
        binaryExpr->op = op;
        binaryExpr->left = left;
        binaryExpr->right = right;
        left = binaryExpr;
    }
    return left;
}
*/

std::shared_ptr<ExpressionNode> Parser::parseExpression() {
    return parseAddSub();
}

std::shared_ptr<ExpressionNode> Parser::parseAddSub() {
    auto node = parseMulDiv();

    while (match(OPERATOR, "+") || match(OPERATOR, "-")) {
        auto op = consume(OPERATOR).value().value;
        auto right = parseMulDiv();
        auto binaryExpr = std::make_shared<BinaryExpressionNode>();
        binaryExpr->op = op;
        binaryExpr->left = node;
        binaryExpr->right = right;
        node = binaryExpr;
    }

    return node;
}

std::shared_ptr<ExpressionNode> Parser::parseMulDiv() {
    auto node = parseExponent();

    while (match(OPERATOR, "*") || match(OPERATOR, "/")) {
        auto op = consume(OPERATOR).value().value;
        auto right = parseExponent();
        auto binaryExpr = std::make_shared<BinaryExpressionNode>();
        binaryExpr->op = op;
        binaryExpr->left = node;
        binaryExpr->right = right;
        node = binaryExpr;
    }

    return node;
}

std::shared_ptr<ExpressionNode> Parser::parseExponent() {
    auto node = parsePrimary();

    while (match(OPERATOR, "^")) {
        auto op = consume(OPERATOR).value().value;
        auto right = parsePrimary();
        auto binaryExpr = std::make_shared<BinaryExpressionNode>();
        binaryExpr->op = op;
        binaryExpr->left = node;
        binaryExpr->right = right;
        node = binaryExpr;
    }

    return node;
}


std::shared_ptr<ExpressionNode> Parser::parsePrimary() {
    std::shared_ptr<IdentifierNode> identifierNode;
    if (match(NUMBER)) {
        return std::make_shared<NumberNode>(consume(NUMBER).value().value);
    } else if (match(IDENTIFIER)) {
        auto identifierToken = consume(IDENTIFIER).value().value;
        identifierNode = std::make_shared<IdentifierNode>(identifierToken);
        if (match(LCHAVE)) {
            // é um indexador
            consume(LCHAVE,"[");
            identifierNode = std::make_shared<IdentifierNode>(identifierToken);
            auto resultado = consume(NUMBER,"",false);
            if (!resultado) {
                // é uma variável indexadora
                resultado = consume(IDENTIFIER);
            }
            identifierNode->posicao = resultado.value().value;
            consume(RCHAVE,"]");
        } else if (match(LPAREN)) {
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
        return identifierNode;
    } else if (match(FUNCTION)) {
        auto functionToken = consume(FUNCTION).value().value;
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

std::optional<Token> Parser::consume(TokenType type, const std::string& value, bool deveExistir) {
    if (match(type, value)) {
        return tokens[pos++];
    }
    if (deveExistir) {
        throw ParserException("Expected token " + tokenTypeName(type) + " but found " + tokens[pos].value);
    }
    return std::nullopt;
}

std::string Parser::tokenTypeName(TokenType type) {
    switch (type) {
        case LINENO: return "LINENO";
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
        std::cout << indentStr << "LetStatementNode: "
        << letStmt->numeroLinha << " >> "
        << letStmt->identifier << std::endl;
        printAST(letStmt->expression, indent + 2);
    } else if (auto printStmt = std::dynamic_pointer_cast<PrintStatementNode>(node)) {
        std::cout << indentStr << "PrintStatementNode: "
        << printStmt->numeroLinha << " >> "
        << std::endl;
        printAST(printStmt->expression, indent + 2);
    } else if (auto gotoStmt = std::dynamic_pointer_cast<GotoStatementNode>(node)) {
        std::cout << indentStr << "GotoStatementNode: "
        << gotoStmt->numeroLinha << " >> "
        << gotoStmt->numeroLinhaDesvio
        << std::endl;
    } else if (auto dimStmt = std::dynamic_pointer_cast<DimStatementNode>(node)) {
        std::cout << indentStr << "DimStatementNode: "
        << dimStmt->nomeVariavel << " >> "
        << dimStmt->numeroOcorrencias
        << std::endl;
    } else if (auto endStmt = std::dynamic_pointer_cast<EndStatementNode>(node)) {
        std::cout << indentStr << "EndStatementNode: "
        << std::endl;
    } else if (auto ifStmt = std::dynamic_pointer_cast<IfStatementNode>(node)) {
        std::cout << indentStr << "IfStatementNode: "
        << ifStmt->operando1 << " " << ifStmt->operadorLogico
        << " " << ifStmt->operando2 << " >> " << ifStmt->numeroLinha;
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
