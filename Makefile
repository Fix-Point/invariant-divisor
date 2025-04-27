CC=${CROSS_COMPILE}gcc
CCFLAGS= -Werror -O3 -g3

all:
	${CC} test_div.c data.c ${CCFLAGS} -o test_div.o
	${CC} test_gettime.c data.c my_clock_gettime.c ${CCFLAGS} -o test_gettime.o

test:
	sudo taskset -c 1 chrt -f 1 ./test_div.o
	sudo taskset -c 1 chrt -f 1 ./test_gettime.o

# Generating Figures
divfigure:
	/usr/bin/time -v ./test_div.o > ./script/rss.txt
	sudo taskset -c 1 chrt -f 1 ./test_div.o > ./script/divtime.txt
	sudo chmod +777 ./script/divtime.txt
	python3 ./script/avgdiv.py ./script/divtime.txt

clean:
	rm -f *.o ./script/*.txt *.pdf