TGs = blank.tg circle.tg comments.tg cross.tg diagonal.tg gray.tg hexagon.tg house.tg square.tg triangle.tg underflow.tg

all : tg.tgz tg tests

tg.tgz : Makefile language.txt tg.c $(TGs)
	tar -czvf tg.tgz $(TGs) language.txt tg.c Makefile

tg : tg.c
	gcc -Wall tg.c -lm -o tg

tests : blank.pgm circle.pgm comments.pgm cross.pgm diagonal.pgm gray.pgm hexagon.pgm house.pgm square.pgm triangle.pgm underflow.pgm

blank.pgm : blank.tg
	cat blank.tg | ./tg 10 10 blank.pgm

circle.pgm : circle.tg
	cat circle.tg | ./tg 500 500 circle.pgm

comments.pgm : comments.tg
	cat comments.tg | ./tg 10 10 comments.pgm

cross.pgm : cross.tg
	cat cross.tg | ./tg 500 500 cross.pgm

diagonal.pgm : diagonal.tg
	cat diagonal.tg | ./tg 500 500 diagonal.pgm

gray.pgm : gray.tg
	cat gray.tg | ./tg 500 500 gray.pgm

hexagon.pgm : hexagon.tg
	cat hexagon.tg | ./tg 500 500 hexagon.pgm

house.pgm : house.tg
	cat house.tg | ./tg 500 500 house.pgm

square.pgm : square.tg
	cat square.tg | ./tg 500 500 square.pgm

triangle.pgm : triangle.tg
	cat triangle.tg | ./tg 500 500 triangle.pgm

underflow.pgm : underflow.tg
	-cat underflow.tg | ./tg 10 10 underflow.pgm
