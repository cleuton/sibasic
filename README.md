# sibasic - Simple Basic Interpreter
![](thumb.png)

[**Cleuton Sampaio**](https://linkedin.com/in/cleutonsampaio)

SiBasic é um interpretador muito simples da linguagem **BASIC**. Na verdade, ele implementa um *subset* da linguagem, 
para fins de estudo de algoritmos, estruturas de dados e compiladores. 

Ele é um projeto totalmente funcional, separado em: 

- **Executor**: Arquivo main.cpp, lê o código BASIC, interpreta e executa.
- **Lexer**: Arquivo lexer.cpp. Um lexer (analisador léxico) é a primeira etapa de um interpretador. 
Ele recebe o código fonte como entrada e o converte em uma sequência de tokens, que são unidades léxicas 
significativas, como palavras-chave, identificadores, números e símbolos. Esses tokens são então passados para o 
analisador sintático para formar a estrutura gramatical do código. Em resumo, o lexer identifica e classifica os 
- elementos do código fonte em tokens que são mais fáceis de serem processados pelo interpretador.
- **Parser**: Arquivo parser.cpp. O parser (analisador sintático) é a segunda etapa de um interpretador. Ele recebe a sequência de tokens produzida pelo lexer e verifica se ela está de acordo com a gramática da linguagem de programação. O parser constrói uma representação hierárquica da estrutura do código, chamada de árvore de análise sintática (AST), que captura a relação entre os diferentes elementos do código, como expressões, declarações e comandos. Essa árvore é então utilizada pelo interpretador para realizar a execução do programa, interpretando cada nó da árvore e realizando as ações correspondentes de acordo com a semântica da linguagem. Em resumo, o parser traduz a sequência de tokens em uma estrutura de dados que o interpretador pode utilizar para entender e executar o código fonte.
- **Interpreter**: Arquivo interpreter.cpp. Um Interpreter (intérprete) é a terceira e última etapa de um interpretador. Ele recebe a estrutura de dados produzida pelo parser, geralmente uma árvore de análise sintática (AST), e executa as instruções representadas por essa estrutura. O intérprete interpreta cada nó da árvore e executa as ações correspondentes de acordo com a semântica da linguagem de programação. Isso envolve avaliar expressões, executar comandos, controlar o fluxo do programa e interagir com o ambiente de execução. Em resumo, o intérprete é responsável por efetivamente executar o código fonte, transformando-o em resultados observáveis ou efeitos no sistema conforme definido pela linguagem.

## Variáveis e vetores

Todas as variáveis são numéricas reais de precisão dupla. Elas podem ser vetores (não matrizes), se as declararmos com o comando **DIM**.

## Comandos BASIC

Cada linha deve conter um e somente um comando BASIC. Todas as linhas devem ser numeradas. 
O **Executor** transforma todos os caracteres em maiúsculas antes de interpretar.

Os seguintes comandos **BASIC** foram implementados: 

- **DIM**: Declara vetores.
- **LET**: Atribui valores ou expressões às variáveis.
- **GOTO**: Desvio incondicional para uma linha. 
- **PRINT**: Exibe o resultado de uma expressão na console.
- **IF**: Desvio condicional para uma linha.
- **END**: Termina o programa.

Este é um dos programas de exemplo: 
```basic
10 LET A = 5
20 LET B = 7
30 IF A < B THEN 50
40 END
50 PRINT A
```

### DIM

Este comando declara um **vetor**. No SiBasic os vetores são BASE 1, ou seja, a primeira posição é 1 e não zero. Sua sintaxe é: 
```basic
DIM <nome do vetor> <número de posições>
```

Um vetor só pode ter uma dimensão.

### LET

Este comando atribui valores ou resultado de **expressões** às variáveis. Estas variáveis podem ser simples ou vetores. Vetores sempre devem ser indexados. Exemplos: 
```basic
10 DIM V 10
20 LET X = 1
30 LET V[X] = EXP(X) * 0.34
40 LET X = X + 1
50 IF X > 10 THEN 70
60 GOTO 30
70 LET X = 1
80 PRINT V[X]
90 LET X = X + 1
100 IF X < 11 THEN 80
```

Se a variável for um vetor, deve ser indexada. O indexador pode ser uma variável ou um número inteiro.

### GOTO

Desvio incondicional: 
```basic
GOTO <número da linha>
```

### PRINT

Exibe o resultado de uma **expressão** na console. Pode ser um número, uma variável ou uma expressão contendo ambos e funções.
```basic
PRINT -3
```

Não podem ser utilizados literais textuais e nem mais de um argumento. 

### IF

Desvio condicional. Este comando compara dois operandos e desvia para a linha especificada caso a condição seja verdadeira. Sua sintaxe é: 
```basic
IF <variável> <operador lógico> <variável ou expressão> THEN <número da linha>
```

O primeiro operando não pode ser uma expressão, devendo ser uma variável. O segundo pode ser uma expressão unária (um só membro).

### END

Termina a execução do programa. Pode haver mais de um comando **END** no seu programa. Ao encontrar este comando, a execução termina.

## Expressões

Os comandos **LET** e **PRINT** admitem expressões, que podem conter: **variáveis**, **operadores aritméticos**, **funções** e **negativo unário**.
Eis alguns exemplos de expressões válidas: 
```basic
A = 5
B=(SIN(A) + 3) ^2
V[X] = X + 50
-(SIN(A) + B)
```

Os operadores aritméticos suportados são: 
- **Adição**: +
- **Subtração**: -
- **Multiplicação**: *
- **Divisão**: /
- **Exponenciação**: ^

## Funções

Esta versão não permite utilizar o comando **DEF FN** para definir funções. Estão implementadas as seguintes funções: 

- **Seno**: SIN
- **Cosseno**: COS
- **Tangente**: TAN
- **Logarítmo natural**: LOG
- **Exponenciação natural**: EXP
- **Raíz quadrada**: SQR
- **Valor absoluto**: ABS

As funções trigonométricas assumem que os ângulos estão em graus (e não em radianos).

## Exemplos legais

Saber se um número é primo com o **crivo de Eratóstenes**: 
```basic
10 DIM A 100
20 LET I = 2
30 IF I > 100 THEN 60
40 LET A[I] = 1
50 LET I = I + 1
55 GOTO 30
60 LET I = 2
70 IF I > 100 THEN 120
80 IF A[I] = 0 THEN 110
90 LET J = I * 2
100 IF J > 100 THEN 110
105 LET A[J] = 0
107 LET J = J + I
108 GOTO 100
110 LET I = I + 1
115 GOTO 70
120 LET N = 31
130 IF A[N] = 0 THEN 160
140 PRINT 1
150 GOTO 170
160 PRINT 0
170 END
```

A linha 120 define um número a ser validado. Se ele for primo, o programa mostrará "1", caso contrário, mostrará "0".