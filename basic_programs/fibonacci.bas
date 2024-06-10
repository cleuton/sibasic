* Calcula a sequência de Fibonacci até o termo desejado
10 LET TERMO=20
20 LET J = 1
30 LET I = 0
40 LET K = 1
50 LET T = I + J
60 LET I = J
70 LET J = T
80 PRINT I
90 LET K = K + 1
100 IF K > TERMO THEN 120
110 GOTO 50
120 END