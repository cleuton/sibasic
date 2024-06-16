#include "Parser.h"
#include <iostream>
#include <sstream>

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

ParserException::ParserException(const std::string& message) : message(message) {}

const char* ParserException::what() const noexcept {
    return message.c_str();
}

Parser::Parser(const std::vector<Token>& tokens, bool jaTemDrawStart) : tokens(tokens), pos(0), jaTemDrawStart(jaTemDrawStart) {}

std::shared_ptr<NoDePrograma> Parser::parse() {
    auto program = std::make_shared<NoDePrograma>();
    std::string numeroLinha = "";
    while (pos < tokens.size() && tokens[pos].type != FIM_DE_LINHA) {
        if (tokens[pos].type == NUMERO_LINHA) {
            numeroLinha = tokens[pos++].value;
            continue;
        }
        std::shared_ptr<NoDeComando> nodePointer = parseComando();
        nodePointer->numeroLinha = numeroLinha;
        program->comandos.push_back(nodePointer);
    }
    return program;
}


std::shared_ptr<NoDeComando> Parser::parseComando() {
    if (encontrar(COMANDO, "LET")) {
        return parseComandoLET();
    } else if (encontrar(COMANDO, "PRINT")) {
        return parseComandoPRINT();
    } else if (encontrar(COMANDO, "GOTO")) {
        return parseComandoGOTO();
    } else if (encontrar(COMANDO, "DIM")) {
        return parseComandoDIM();
    } else if (encontrar(COMANDO, "END")) {
        return parseComandoEND();
    } else if (encontrar(COMANDO, "IF")) {
        return parseComandoIF();
    } else if (encontrar(COMANDO, "INPUT")) {
        return parseComandoINPUT();
    } else if (encontrar(COMANDO,"DRAW" )) {
        return parseComandoDRAW();
    } else if (encontrar(COMANDO, "PLOT")) {
        return parseComandoPLOT();
    } else if (encontrar(COMANDO, "LINE")) {
        return parseComandoLINE();
    } else if (encontrar(COMANDO, "RECTANGLE")) {
        return parseComandoRECTANGLE();
    } else {
        throw ParserException("Unexpected command: " + tokens[pos].value);
    }
}

std::shared_ptr<NoDoComandoLET> Parser::parseComandoLET() {
    consumir(COMANDO, "LET");
    auto letStmt = std::make_shared<NoDoComandoLET>();
    letStmt->identificador = consumir(IDENTIFICADOR).value().value;
    auto token = consumir(CHAVE_ESQUERDA, "", false);
    if (token) {
        // É uma variável indexada. Pode ter um INDENTIFIER ou um NUMBER e um RCHAVE
        auto resultado = consumir(NUMERO, "",false);
        if (!resultado.has_value()) {
            // é uma variável indexando.
            auto resultado2 = consumir(IDENTIFICADOR);
            if (resultado2.has_value()) {
                letStmt->posicao = resultado2.value().value;
            }
        } else {
            letStmt->posicao = resultado.value().value;
        }
        consumir(CHAVE_DIREITA);
    }
    consumir(OPERADOR, "=");
    letStmt->expressao = parseExpressao();
    return letStmt;
}

std::shared_ptr<NoDoComandoDIM> Parser::parseComandoDIM() {
    consumir(COMANDO, "DIM");
    auto dimStmt = std::make_shared<NoDoComandoDIM>();
    dimStmt->nomeVariavel = consumir(IDENTIFICADOR).value().value;
    dimStmt->numeroOcorrencias = std::stoi(consumir(NUMERO).value().value);
    return dimStmt;
}

std::shared_ptr<NoDoComandoPRINT> Parser::parseComandoPRINT() {
    consumir(COMANDO, "PRINT");
    auto printStmt = std::make_shared<NoDoComandoPRINT>();
    if (encontrar(LITERAL_TEXTO, "")) {
        // É um literal
        printStmt->printLiteral = true;
        printStmt->literal = tokens[pos].value;
        consumir(LITERAL_TEXTO);
        return printStmt;
    }
    printStmt->expressao = parseExpressao();
    return printStmt;
}

std::shared_ptr<NoDoComandoGOTO> Parser::parseComandoGOTO() {
    consumir(COMANDO, "GOTO");
    auto gotoStmt = std::make_shared<NoDoComandoGOTO>();
    gotoStmt->numeroLinhaDesvio = consumir(NUMERO).value().value;
    return gotoStmt;
}

std::shared_ptr<NoDoComandoEND> Parser::parseComandoEND() {
    consumir(COMANDO, "END");
    auto endStmt = std::make_shared<NoDoComandoEND>();
    return endStmt;
}

std::shared_ptr<NoDoComandoIF> Parser::parseComandoIF() {
    consumir(COMANDO, "IF");
    auto ifStmt = std::make_shared<NoDoComandoIF>();
    ifStmt->operando1 = parsePrimaria();
    auto retorno = consumir(OPERADOR, "=", false);
    if (!retorno.has_value()) {
        retorno = consumir(OPERADOR, ">", false);
        if (!retorno.has_value()) {
            retorno = consumir(OPERADOR, "<", false);
            if (!retorno.has_value()) {
                throw ParserException("IF sem operador lógico");
            }
        }
    }
    ifStmt->operadorLogico = retorno.value().value;
    ifStmt->operando2 = parsePrimaria();
    consumir(IDENTIFICADOR, "THEN");
    ifStmt->numeroLinha = consumir(NUMERO).value().value;
    return ifStmt;
}

std::shared_ptr<NoDoComandoINPUT> Parser::parseComandoINPUT() {
    consumir(COMANDO, "INPUT");
    auto inputStmt = std::make_shared<NoDoComandoINPUT>();
    inputStmt->identificador = consumir(IDENTIFICADOR).value().value;
    return inputStmt;
}

std::shared_ptr<NoDoComandoDRAW> Parser::parseComandoDRAW() {
    // Há duas formas: DRAW START e DRAW FINISH. Elas tem que fazer par
    consumir(COMANDO, "DRAW");
    auto drawStmt = std::make_shared<NoDoComandoDRAW>();
    if (encontrar(IDENTIFICADOR, "START")) {
        // É um DRAW START
        if (jaTemDrawStart) {
            throw ParserException("Dois DRAW START!");
        }
        drawStmt->tipo = consumir(IDENTIFICADOR).value().value;
        jaTemDrawStart = true;

        drawStmt->altura = parseExpressao();
        consumir(VIRGULA);
        drawStmt->largura = parseExpressao();
    } else {
        // É um DRAW END
        if (encontrar(IDENTIFICADOR, "FINISH")) {
            if (!jaTemDrawStart) {
                throw ParserException("Draw FINISH sem DRAW START!");
            }
            jaTemDrawStart = false;
            drawStmt->tipo = consumir(IDENTIFICADOR, "FINISH").value().value;
        }
    }
    return drawStmt;
}

std::shared_ptr<NoDoComandoPLOT> Parser::parseComandoPLOT() {
    if (!jaTemDrawStart) {
        throw ParserException("PLOT sem DRAW START!");
    }
    consumir(COMANDO, "PLOT");
    auto plotStmt = std::make_shared<NoDoComandoPLOT>();
    plotStmt->posicaoX = parseExpressao();
    consumir(VIRGULA);
    plotStmt->posicaoY = parseExpressao();
    consumir(VIRGULA);
    plotStmt->espessura = parseExpressao();
    consumir(VIRGULA);
    plotStmt->cor = consumir(IDENTIFICADOR).value().value;
    if (encontrar(VIRGULA, ",")) {
        consumir(VIRGULA);
        if (encontrar(IDENTIFICADOR, "FILL")) {
            plotStmt->preencher = true;
            consumir(IDENTIFICADOR);
        } else {
            plotStmt->preencher = false;
        }
    }
    return plotStmt;
}

std::shared_ptr<NoDoComandoLINE> Parser::parseComandoLINE() {
    if (!jaTemDrawStart) {
        throw ParserException("LINE sem DRAW START!");
    }
    consumir(COMANDO, "LINE");
    auto lineStmt = std::make_shared<NoDoComandoLINE>();
    lineStmt->xInicial = parseExpressao();
    consumir(VIRGULA);
    lineStmt->yInicial = parseExpressao();
    consumir(VIRGULA);
    lineStmt->xFinal = parseExpressao();
    consumir(VIRGULA);
    lineStmt->yFinal = parseExpressao();
    consumir(VIRGULA);
    lineStmt->cor = consumir(IDENTIFICADOR).value().value;
    return lineStmt;
}

std::shared_ptr<NoDoComandoRECTANGLE> Parser::parseComandoRECTANGLE() {
    if (!jaTemDrawStart) {
        throw ParserException("RECTANGLE sem DRAW START!");
    }
    consumir(COMANDO, "RECTANGLE");
    auto rectStmt = std::make_shared<NoDoComandoRECTANGLE>();
    rectStmt->xCantoSuperiorEsquerdo = parseExpressao();
    consumir(VIRGULA);
    rectStmt->yCantoSuperiorEsquerdo = parseExpressao();
    consumir(VIRGULA);
    rectStmt->xCantoInferiorDireito = parseExpressao();
    consumir(VIRGULA);
    rectStmt->yCantoInferiorDireito = parseExpressao();
    consumir(VIRGULA);
    rectStmt->cor = consumir(IDENTIFICADOR).value().value;
    if (tokens.size() == 14) {
        consumir(VIRGULA);
        if (encontrar(IDENTIFICADOR, "FILL")) {
            rectStmt->preencher = true;
            consumir(IDENTIFICADOR);
        } else {
            rectStmt->preencher = false;
        }
    }
    return rectStmt;
}

std::shared_ptr<NoDeExpressao> Parser::parseExpressao() {
    return parseSomaSub();
}

std::shared_ptr<NoDeExpressao> Parser::parseSomaSub() {
    auto node = parseMultDiv();

    while (encontrar(OPERADOR, "+") || encontrar(OPERADOR, "-")) {
        auto op = consumir(OPERADOR).value().value;
        auto right = parseMultDiv();
        auto binaryExpr = std::make_shared<NoDeExpressaoBinaria>();
        binaryExpr->op = op;
        binaryExpr->left = node;
        binaryExpr->right = right;
        node = binaryExpr;
    }

    return node;
}

std::shared_ptr<NoDeExpressao> Parser::parseMultDiv() {
    auto node = parseExponenciacao();

    while (encontrar(OPERADOR, "*") || encontrar(OPERADOR, "/")) {
        auto op = consumir(OPERADOR).value().value;
        auto right = parseExponenciacao();
        auto binaryExpr = std::make_shared<NoDeExpressaoBinaria>();
        binaryExpr->op = op;
        binaryExpr->left = node;
        binaryExpr->right = right;
        node = binaryExpr;
    }

    return node;
}

std::shared_ptr<NoDeExpressao> Parser::parseExponenciacao() {
    auto node = parsePrimaria();

    while (encontrar(OPERADOR, "^")) {
        auto op = consumir(OPERADOR).value().value;
        auto right = parsePrimaria();
        auto binaryExpr = std::make_shared<NoDeExpressaoBinaria>();
        binaryExpr->op = op;
        binaryExpr->left = node;
        binaryExpr->right = right;
        node = binaryExpr;
    }

    return node;
}


std::shared_ptr<NoDeExpressao> Parser::parsePrimaria() {
    std::shared_ptr<NoDeIdentificador> identifierNode;
    if (encontrar(NUMERO)) {
        return std::make_shared<NoDeNumero>(consumir(NUMERO).value().value);
    } else if (encontrar(IDENTIFICADOR)) {
        auto identifierToken = consumir(IDENTIFICADOR).value().value;
        identifierNode = std::make_shared<NoDeIdentificador>(identifierToken);
        if (encontrar(CHAVE_ESQUERDA)) {
            // é um indexador
            consumir(CHAVE_ESQUERDA,"[");
            identifierNode = std::make_shared<NoDeIdentificador>(identifierToken);
            auto resultado = consumir(NUMERO,"",false);
            if (!resultado) {
                // é uma variável indexadora
                resultado = consumir(IDENTIFICADOR);
            }
            identifierNode->posicao = resultado.value().value;
            consumir(CHAVE_DIREITA,"]");
        } else if (encontrar(PARENTESIS_ESQUERDO)) {
            auto functionCall = std::make_shared<NoDeFuncao>(identifierToken);
            consumir(PARENTESIS_ESQUERDO);
            if (!encontrar(PARENTESIS_DIREITO)) {
                functionCall->argumentos.push_back(parseExpressao());
                while (encontrar(VIRGULA)) {
                    consumir(VIRGULA);
                    functionCall->argumentos.push_back(parseExpressao());
                }
            }
            consumir(PARENTESIS_DIREITO);
            return functionCall;
        }
        return identifierNode;
    } else if (encontrar(FUNCAO)) {
        auto functionToken = consumir(FUNCAO).value().value;
        auto functionCall = std::make_shared<NoDeFuncao>(functionToken);
        consumir(PARENTESIS_ESQUERDO);
        // RND não tem argumentos. Mas precisamos generalizar esse comportamento
        if (functionToken == "RND") {
            consumir(PARENTESIS_DIREITO);
            return functionCall;
        }
        functionCall->argumentos.push_back(parseExpressao());
        while (encontrar(VIRGULA)) {
            consumir(VIRGULA);
            functionCall->argumentos.push_back(parseExpressao());
        }
        consumir(PARENTESIS_DIREITO);
        return functionCall;
    } else if (encontrar(PARENTESIS_ESQUERDO)) {
        consumir(PARENTESIS_ESQUERDO);
        auto expr = parseExpressao();
        consumir(PARENTESIS_DIREITO);
        return expr;
    } else {
        throw ParserException("Token inesperado: " + tokens[pos].value);
    }
}

bool Parser::encontrar(TokenType type, const std::string& value) {
    if (pos < tokens.size() && tokens[pos].type == type && (value.empty() || tokens[pos].value == value)) {
        return true;
    }
    return false;
}

std::optional<Token> Parser::consumir(TokenType type, const std::string& value, bool deveExistir) {
    if (encontrar(type, value)) {
        return tokens[pos++];
    }
    if (deveExistir) {
        throw ParserException("Token esperado: " + tokenTypeName(type) + " encontrado: " + tokens[pos].value);
    }
    return std::nullopt;
}

std::string Parser::tokenTypeName(TokenType type) {
    switch (type) {
        case NUMERO_LINHA: return "NUMERO_LINHA";
        case COMANDO: return "COMANDO";
        case IDENTIFICADOR: return "IDENTIFICADOR";
        case NUMERO: return "NUMERO";
        case OPERADOR: return "OPERADOR";
        case FUNCAO: return "FUNCAO";
        case PARENTESIS_ESQUERDO: return "PARENTESIS_ESQUERDO";
        case PARENTESIS_DIREITO: return "PARENTESIS_DIREITO";
        case VIRGULA: return "VIRGULA";
        case FIM_DE_LINHA: return "FIM_DE_LINHA";
        case ASPAS_DUPLAS: return "ASPAS_DUPLAS";
        default: return "DESCONHECIDO";
    }
}

void mostrarAST(const std::shared_ptr<NoDaAST>& node, int indent) {
    std::string indentStr(indent, ' ');
    if (auto program = std::dynamic_pointer_cast<NoDePrograma>(node)) {
        std::cout << indentStr << "NoDePrograma" << std::endl;
        for (const auto& stmt : program->comandos) {
            mostrarAST(stmt, indent + 2);
        }
    } else if (auto letStmt = std::dynamic_pointer_cast<NoDoComandoLET>(node)) {
        std::cout << indentStr << "NoDoComandoLET: "
        << letStmt->numeroLinha << " >> "
        << letStmt->identificador << std::endl;
        mostrarAST(letStmt->expressao, indent + 2);
    } else if (auto printStmt = std::dynamic_pointer_cast<NoDoComandoPRINT>(node)) {
        std::cout << indentStr << "NoDoComandoPrint: "
        << printStmt->numeroLinha << " >> "
        << " literal: " << printStmt->printLiteral
        << std::endl;
        mostrarAST(printStmt->expressao, indent + 2);
    } else if (auto gotoStmt = std::dynamic_pointer_cast<NoDoComandoGOTO>(node)) {
        std::cout << indentStr << "NoDoComandoGOTO: "
        << gotoStmt->numeroLinha << " >> "
        << gotoStmt->numeroLinhaDesvio
        << std::endl;
    } else if (auto dimStmt = std::dynamic_pointer_cast<NoDoComandoDIM>(node)) {
        std::cout << indentStr << "NoDoComandoDIM: "
        << dimStmt->nomeVariavel << " >> "
        << dimStmt->numeroOcorrencias
        << std::endl;
    } else if (auto endStmt = std::dynamic_pointer_cast<NoDoComandoEND>(node)) {
        std::cout << indentStr << "NoDoComandoEND: "
        << std::endl;
    } else if (auto ifStmt = std::dynamic_pointer_cast<NoDoComandoIF>(node)) {
        std::cout << indentStr << "NoDoComandoIF: "
        << ifStmt->operando1 << " " << ifStmt->operadorLogico
        << " " << ifStmt->operando2 << " >> " << ifStmt->numeroLinha;
    } else if (auto inputStmt = std::dynamic_pointer_cast<NoDoComandoINPUT>(node)) {
        std::cout << indentStr << "NoDoComandoINPUT: "
                  << inputStmt->identificador << std::endl;
    } else if (auto drawStmt = std::dynamic_pointer_cast<NoDoComandoDRAW>(node)) {
        std::cout << indentStr << "NoDoComandoDRAW: "
                  << drawStmt->tipo << ", "
                  << drawStmt->altura << ", " << drawStmt->largura
                  << std::endl;
    } else if (auto plotStmt = std::dynamic_pointer_cast<NoDoComandoPLOT>(node)) {
        std::cout << indentStr << "NoDoComandoPLOT: "
                  << plotStmt->posicaoX << ", "
                  << plotStmt->posicaoY << ", " << plotStmt->cor
                  << ", " << plotStmt->preencher
                  << std::endl;
    } else if (auto lineStmt = std::dynamic_pointer_cast<NoDoComandoLINE>(node)) {
        std::cout << indentStr << "NoDoComandoLINE: "
                  << lineStmt->xInicial << ", "
                  << lineStmt->yInicial << ", " << lineStmt->xFinal
                  << ", " << lineStmt->yFinal << ", " << lineStmt->cor
                  << std::endl;
    } else if (auto rectStmt = std::dynamic_pointer_cast<NoDoComandoRECTANGLE>(node)) {
        std::cout << indentStr << "NoDoComandoRECTANGLE: "
                  << rectStmt->xCantoSuperiorEsquerdo << ", "
                  << rectStmt->yCantoSuperiorEsquerdo << ", " << rectStmt->xCantoInferiorDireito
                  << ", " << rectStmt->yCantoInferiorDireito << ", " << rectStmt->cor
                  << ", " << rectStmt->preencher
                  << std::endl;
    } else if (auto binaryExpr = std::dynamic_pointer_cast<NoDeExpressaoBinaria>(node)) {
        std::cout << indentStr << "NoDeExpressaoBinaria: " << binaryExpr->op << std::endl;
        mostrarAST(binaryExpr->left, indent + 2);
        mostrarAST(binaryExpr->right, indent + 2);
    } else if (auto number = std::dynamic_pointer_cast<NoDeNumero>(node)) {
        std::cout << indentStr << "NoDeNumero: " << number->value << std::endl;
    } else if (auto identifier = std::dynamic_pointer_cast<NoDeIdentificador>(node)) {
        std::cout << indentStr << "NoDeIdentificador: " << identifier->name << std::endl;
    } else if (auto functionCall = std::dynamic_pointer_cast<NoDeFuncao>(node)) {
        std::cout << indentStr << "NoDeFuncao: " << functionCall->nomeDaFuncao << std::endl;
        for (const auto& arg : functionCall->argumentos) {
            mostrarAST(arg, indent + 2);
        }
    }
}
