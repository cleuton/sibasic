10 DIM V 10
20 LET X = 1
30 LET V[X] = EXP(X) + 1
40 LET X = X + 1
50 IF X > 10 THEN 70
60 GOTO 30
70 LET X = 1
80 PRINT V[X]
90 LET X = X + 1
100 IF X < 11 THEN 80