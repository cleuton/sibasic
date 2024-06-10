#ifndef TOKEN_H
#define TOKEN_H

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

#include <string>

enum TokenType {
    COMANDO, IDENTIFICADOR, NUMERO, OPERADOR, FUNCAO, PARENTESIS_ESQUERDO, PARENTESIS_DIREITO,
    VIRGULA, FIM_DE_LINHA, NUMERO_LINHA, CHAVE_DIREITA, CHAVE_ESQUERDA, ASPAS_DUPLAS, LITERAL_TEXTO
};

struct Token {
    TokenType type;
    std::string value;
};

#endif // TOKEN_H