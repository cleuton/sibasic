#ifndef PARSER_H
#define PARSER_H

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

#include "Token.h"
#include <memory>
#include <vector>
#include <string>
#include <exception>
#include <optional>

class NoDaAST {
public:
    virtual ~NoDaAST() = default;
};

using NoDaASTPtr = std::shared_ptr<NoDaAST>;

class NoDePrograma : public NoDaAST {
public:
    std::vector<NoDaASTPtr> comandos;
};

class NoDeComando : public NoDaAST {
public:
    std::string numeroLinha;
    virtual ~NoDeComando() = default;
};

class NoDoComandoLET : public NoDeComando {
public:
    std::string identificador;
    std::string posicao;
    NoDaASTPtr expressao;
};

class NoDoComandoPRINT : public NoDeComando {
public:
    bool printLiteral;
    NoDaASTPtr expressao;
    std::string literal;
};

class NoDoComandoGOTO : public NoDeComando {
public:
    std::string numeroLinhaDesvio;
};

class NoDoComandoDIM : public NoDeComando {
public:
    std::string nomeVariavel;
    int numeroOcorrencias = 0;
};

class NoDoComandoEND : public NoDeComando {

};

class NoDoComandoIF : public NoDeComando {
public:
    NoDaASTPtr operando1;
    std::string operadorLogico;
    NoDaASTPtr operando2;
    std::string numeroLinha;
};

class NoDoComandoINPUT : public NoDeComando {
public:
    std::string identificador;
};

class NoDoComandoDRAW : public NoDeComando {
public:
    std::string tipo; // Begin ou end
    NoDaASTPtr largura; // largura e altura podem ser variáveis ou números
    NoDaASTPtr altura;
};

class NoDoComandoPLOT : public NoDeComando {
public:
    // Os atributos exceto o "preencher" podem ser variáveis ou números
    NoDaASTPtr posicaoX;
    NoDaASTPtr posicaoY;
    NoDaASTPtr espessura;
    std::string cor;
    bool preencher;
};

class NoDoComandoLINE : public NoDeComando {
public:
    // Os atributos podem ser variáveis ou números
    NoDaASTPtr xInicial;
    NoDaASTPtr yInicial;
    NoDaASTPtr xFinal;
    NoDaASTPtr yFinal;
    std::string cor;
};

class NoDoComandoRECTANGLE : public NoDeComando {
public:
    // Os atributos exceto o "preencher" podem ser variáveis ou números
    NoDaASTPtr xCantoSuperiorEsquerdo;
    NoDaASTPtr yCantoSuperiorEsquerdo;
    NoDaASTPtr xCantoInferiorDireito;
    NoDaASTPtr yCantoInferiorDireito;
    std::string cor;
    bool preencher;
};

class NoDeExpressao : public NoDaAST {
public:
    virtual ~NoDeExpressao() = default;
};

class NoDeFuncao : public NoDeExpressao {
public:
    std::string nomeDaFuncao;
    std::vector<NoDaASTPtr> argumentos;

    explicit NoDeFuncao(const std::string& nomeDeFuncao) : nomeDaFuncao(nomeDeFuncao) {}
};



class NoDeExpressaoBinaria : public NoDeExpressao {
public:
    std::string op;
    NoDaASTPtr left;
    NoDaASTPtr right;
};



class NoDeNumero : public NoDeExpressao {
public:
    std::string value;
    explicit NoDeNumero(const std::string& value) : value(value) {}
};

class NoDeIdentificador : public NoDeExpressao {
public:
    std::string name;
    std::string posicao;
    explicit NoDeIdentificador(const std::string& name) : name(name) {}
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
    explicit Parser(const std::vector<Token>& tokens, bool jaTemDrawStart);

    std::shared_ptr<NoDePrograma> parse();
    std::string tokenTypeName(TokenType type);
    bool jaTemDrawStart;
private:
    std::vector<Token> tokens;
    size_t pos;
    std::shared_ptr<NoDeComando> parseComando();
    std::shared_ptr<NoDoComandoLET> parseComandoLET();
    std::shared_ptr<NoDoComandoPRINT> parseComandoPRINT();
    std::shared_ptr<NoDoComandoGOTO> parseComandoGOTO();
    std::shared_ptr<NoDoComandoDIM> parseComandoDIM();
    std::shared_ptr<NoDoComandoEND> parseComandoEND();
    std::shared_ptr<NoDoComandoIF> parseComandoIF();
    std::shared_ptr<NoDoComandoINPUT> parseComandoINPUT();
    std::shared_ptr<NoDoComandoDRAW> parseComandoDRAW();
    std::shared_ptr<NoDoComandoPLOT> parseComandoPLOT();
    std::shared_ptr<NoDoComandoLINE> parseComandoLINE();
    std::shared_ptr<NoDoComandoRECTANGLE> parseComandoRECTANGLE();
    std::shared_ptr<NoDeExpressao> parseExpressao();
    std::shared_ptr<NoDeExpressao> parseSomaSub();
    std::shared_ptr<NoDeExpressao> parseMultDiv();
    std::shared_ptr<NoDeExpressao> parseExponenciacao();
    std::shared_ptr<NoDeExpressao> parsePrimaria();

    bool encontrar(TokenType type, const std::string& value = "");
    std::optional<Token> consumir(TokenType type, const std::string& value = "", bool deveExistir = true);


};

void

 mostrarAST(const std::shared_ptr<NoDaAST>& node, int indent = 0);

#endif // PARSER_H