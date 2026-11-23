CC = gcc
FLAGS = -O3 -fopenmp
HEADER = functions.h
MAIN = main.c

	
dcs: EXTRA = miniMaxDFS.c pawnsSequentalDFS.c
dcs: OUT = comp/dcs
dcs: build

dct: EXTRA = miniMaxDFS.c pawnsThreadedDFS.c
dct: OUT = comp/dct
dct: build

bcs: EXTRA = pawnsSequentalBFS.c
bcs: OUT = comp/bcs
bcs: build

build:
	$(CC) $(FLAGS) $(HEADER) $(MAIN) $(EXTRA) -o $(OUT)