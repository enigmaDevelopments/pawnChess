CC = gcc
FLAGS = -O3
HEADER = functions.h structs.h
MAIN = main.c

	
dcs: EXTRA = miniMaxDFS.c pawnsSequentalDFS.c dfs.h
dcs: OUT = comp/dcs
dcs: build

dct: EXTRA = -fopenmp miniMaxDFS.c pawnsThreadedDFS.c dfs.h 
dct: OUT = comp/dct
dct: build

bcs: EXTRA = pawnsBFS.c bfs.h forwardBSF.c scoreBSF.c backwardBSF.c 
bcs: OUT = comp/bcs
bcs: build

bct: EXTRA = -fopenmp pawnsBFS.c bfs.h forwardBSF.c scoreBSF.c backwardBSF.c 
bct: OUT = comp/bct
bct: build

build:
	$(CC) $(FLAGS) $(HEADER) $(MAIN) $(EXTRA) -o $(OUT)