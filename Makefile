CC = gcc
FLAGS = -O3 -fopenmp
HEADER = functions.h
MAIN = main.c

	
dcs: EXTRA = miniMaxDFS.c pawnsSequentalDFS.c
dcs: OUT = comp/dcs
dcs: all

dct: EXTRA = miniMaxDFS.c pawnsThreadedDFS.c
dct: OUT = comp/dct
dct: all

all:
	$(CC) $(FLAGS) $(HEADER) $(MAIN) $(EXTRA) -o $(OUT)