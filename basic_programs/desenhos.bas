10 LET X = 10
20 LET Y = 10
30 DRAW START 500, 500
* PLOT <x>,<y>,<raio>,<cor>,[FILL]
40 PLOT X, Y, 1, BLUE
* LINE <x>,<y>,<x2>,<y2>,<cor>
50 LINE X, Y, 100, 100, RED
* RECTANGLE <topleft x>, <topleft y>, <bottomright x>, <bottomright y>, <cor>,[FILL]
60 RECTANGLE X, Y, 100, 100, GREEN
70 DRAW FINISH
