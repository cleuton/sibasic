#ifndef SIBASIC_INTERPRETER_H
#define SIBASIC_INTERPRETER_H
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
    double processarFuncao(const std::string& nomeDaFuncao, double argumento, bool temArgumento = true);
    int getPosicao(const std::string& varivavel, const std::string& indexador);
};
#endif //SIBASIC_INTERPRETER_H
