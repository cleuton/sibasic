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
    void executar(const std::shared_ptr<NoDePrograma>& programa);
    int executarComando(const std::shared_ptr<NoDaAST>& comando, const std::shared_ptr<NoDePrograma>& programa);
    double avaliarExpressao(const std::shared_ptr<NoDaAST>& expressao);

private:
    // Função para converter graus em radianos
    double grausParaRadianos(double degrees);
    std::unordered_map<std::string, std::vector<double>> variables;
    std::unordered_map<std::string, int> vetores;
    // Função para processar chamadas de função
    double processarFuncao(const std::string& nomeDaFuncao, double argumento);
    int getPosicao(const std::string& varivavel, const std::string& indexador);
};
#endif //SIBASIC_INTERPRETER_H
