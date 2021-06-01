TGs = blank.tg circle.tg comments.tg cross.tg diagonal.tg gray.tg hexagon.tg house.tg pentagon.tg square.tg triangle.tg tripofobia.tg underflow.tg

all : tg.tgz tg tests

tg.tgz : Makefile language.txt license.txt tg.c $(TGs)
	tar -czvf tg.tgz $(TGs) language.txt tg.c Makefile

tg : tg.c
	gcc -Wall tg.c -lm -o tg

tests : blank.pgm circle.pgm comments.pgm cross.pgm diagonal.pgm gray.pgm hexagon.pgm house.pgm pentagon.pgm square.pgm triangle.pgm tripofobia.pgm underflow.pgm

clean:
	-rm tg blank.pgm circle.pgm comments.pgm cross.pgm diagonal.pgm gray.pgm hexagon.pgm house.pgm pentagon.pgm square.pgm triangle.pgm tripofobia.pgm underflow.pgm

blank.pgm : tg blank.tg
	-rm blank.pgm
	cat blank.tg | ./tg 100 100 blank.pgm

circle.pgm : tg circle.tg
	-rm circle.pgm
	cat circle.tg | ./tg 500 500 circle.pgm

comments.pgm : tg comments.tg
	-rm comments.pgm
	cat comments.tg | ./tg 10 10 comments.pgm

cross.pgm : tg cross.tg
	-rm cross.pgm
	cat cross.tg | ./tg 500 500 cross.pgm

diagonal.pgm : tg diagonal.tg
	-rm diagonal.pgm
	cat diagonal.tg | ./tg 500 500 diagonal.pgm

gray.pgm : tg gray.tg
	-rm gray.pgm
	cat gray.tg | ./tg 500 500 gray.pgm

hexagon.pgm : tg hexagon.tg
	-rm hexagon.pgm
	cat hexagon.tg | ./tg 500 500 hexagon.pgm

house.pgm : tg house.tg
	-rm house.pgm
	cat house.tg | ./tg 500 500 house.pgm

square.pgm : tg square.tg
	-rm square.pgm
	cat square.tg | ./tg 500 500 square.pgm

triangle.pgm : tg triangle.tg
	-rm triangle.pgm
	cat triangle.tg | ./tg 500 500 triangle.pgm

underflow.pgm : tg underflow.tg
	-rm underflow.pgm
	-cat underflow.tg | ./tg 10 10 underflow.pgm

tripofobia.pgm: tg tripofobia.tg
	-rm tripofobia.pgm
	cat tripofobia.tg | ./tg 640 480 tripofobia.pgm

pentagon.pgm : tg pentagon.tg
	-rm pentagon.pgm
	cat pentagon.tg | ./tg 640 480 pentagon.pgm
