CC=${CROSS_COMPILE}gcc
CCFLAGS= -Werror -O3 -g3

all:
	${CC} test_div.c data.c ${CCFLAGS} -o test_div.o
	${CC} test_gettime.c data.c my_clock_gettime.c ${CCFLAGS} -o test_gettime.o

all32:
	${CC} -m32 test_div.c data.c ${CCFLAGS} -o test_div32.o
	${CC} -m32 test_gettime.c data.c my_clock_gettime.c ${CCFLAGS} -o test_gettime32.o

test:
	mkdir -p log
	sudo taskset -c 1 chrt -f 1 ./test_div.o
	sudo taskset -c 1 chrt -f 1 ./test_gettime.o

# Generating Figures
divfigure:
	mkdir -p log
	/usr/bin/time -v ./test_div.o > ./log/divrss.txt
	sudo taskset -c 1 chrt -f 1 ./test_div.o > ./log/divtime.txt
	sudo chmod +777 ./log/*.txt
	python3 ./script/avgdiv.py ./log/divtime.txt
	python3 ./script/latencydiv.py ./log

divfigure_nonroot:
	mkdir -p log
	/usr/bin/time ./test_div.o > ./log/divrss.txt
	./test_div.o > ./log/divtime.txt
	python3 ./script/avgdiv.py ./log/divtime.txt
	python3 ./script/latencydiv.py ./log

clean:
	rm -f *.o ./log/* *.pdf *.png
