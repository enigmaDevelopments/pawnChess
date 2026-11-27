#include "functions.h"
#include "bfs.h"
#ifdef _OPENMP
#include <omp.h>
#endif


void forwardPass(struct Board** boardsPtr,int** moveCounts,int* moveCountsCount,int numBoards,int depth, bool p1Turn){
    struct Board* boards = *boardsPtr;
    struct Moves* moves;
    moveCounts[0] = malloc(sizeof(int) * 2);

    for (int d = 0; d < depth; d++){
        moves = (struct Moves*) malloc(sizeof(struct Moves) * numBoards);
        moveCounts[d] = malloc(sizeof(int) * (numBoards+1));
        {
            moveCounts[d][0] = 0;
            int prefix = 0;
            int count;
            if (p1Turn){
                #pragma omp parallel for simd reduction(inscan, +:prefix) private(count) if (numBoards != 1)
                for (int i = 0; i < numBoards; i++){
                    if (boards[i].p1 & 0xFF00000000000000 || boards[i].p2 & 0x00000000000000FF || !boards[i].p1 || !boards[i].p2)
                        moves[i] = (struct Moves) {0, 0, 0};
                    else
                        moves[i] = getMovesP1(boards[i]);
                    count = __builtin_popcountll(moves[i].strightMoves) + __builtin_popcountll(moves[i].leftMoves) + __builtin_popcountll(moves[i].rightMoves);
                    if (count == 0)
                        count = 1;
                    prefix += count;
                    #pragma omp scan inclusive(prefix)
                    moveCounts[d][i+1] = prefix;
                }
            }
            else{
                #pragma omp parallel for simd reduction(inscan, +:prefix) private(count) if (numBoards != 1)
                for (int i = 0; i < numBoards; i++){
                    if (boards[i].p1 & 0xFF00000000000000 || boards[i].p2 & 0x00000000000000FF || !boards[i].p1 || !boards[i].p2)
                        moves[i] = (struct Moves) {0, 0, 0};
                    else
                        moves[i] = getMovesP2(boards[i]);
                    count = __builtin_popcountll(moves[i].strightMoves) + __builtin_popcountll(moves[i].leftMoves) + __builtin_popcountll(moves[i].rightMoves);
                    if (count == 0)
                        count = 1;
                    prefix += count;
                    #pragma omp scan inclusive(prefix)
                    moveCounts[d][i+1] = prefix;   
                }
            }
        }

        moveCountsCount[d+1] = moveCounts[d][numBoards];
        struct Board* newBoards = malloc(sizeof(struct Board) * moveCountsCount[d+1]);

        if (p1Turn){
            #pragma omp parallel for if (numBoards != 1)
            for (int i = 0; i < numBoards; i++){
                if (moves[i].leftMoves == 0 && moves[i].rightMoves == 0 && moves[i].strightMoves == 0){
                    newBoards[moveCounts[d][i]] = boards[i];
                    printf("%d\n",d);
                    continue;
                }
                int offset = 0;
                #pragma unroll
                for (short j = 0; j < 3; j++){
                    unsigned long long *move = (unsigned long long *)&moves[i] + j;
                    while (*move){
                        long long k = 1ULL << __builtin_ctzll(*move);
                        *move ^= k;
                        if (j == 0){
                            unsigned long long newP1 = boards[i].p1 | k;
                            newP1 ^= k >> 8;
                            newBoards[moveCounts[d][i] + offset++] = (struct Board){newP1, boards[i].p2};
                        }
                        else if (j == 1){
                            unsigned long long newP1 = boards[i].p1 | k;
                            newP1 ^= k >> 9;
                            unsigned long long newP2 = boards[i].p2 ^ k;
                            newBoards[moveCounts[d][i] + offset++] = (struct Board){newP1, newP2};
                        }
                        else if (j == 2){
                            unsigned long long newP1 = boards[i].p1 | k;
                            newP1 ^= k >> 7;
                            unsigned long long newP2 = boards[i].p2 ^ k;
                            newBoards[moveCounts[d][i] + offset++] = (struct Board){newP1, newP2};
                        }
                    }
                }
            }
        }
        else{
            #pragma omp parallel for if (numBoards != 1)
            for (int i = 0; i < numBoards; i++){
                if (moves[i].leftMoves == 0 && moves[i].rightMoves == 0 && moves[i].strightMoves == 0){
                    newBoards[moveCounts[d][i]] = boards[i];
                    printf("%d\n",d);
                    continue;
                }
                int offset = 0;
                #pragma unroll
                for (short j = 0; j < 3; j++){
                    unsigned long long *move = (unsigned long long *)&moves[i] + j;
                    while (*move){
                        long long k = 1ULL << __builtin_ctzll(*move);
                        *move ^= k;
                        if (j == 0){
                            unsigned long long newP2 = boards[i].p2 | k;
                            newP2 ^= k << 8;
                            newBoards[moveCounts[d][i]+ offset++] = (struct Board){boards[i].p1, newP2};
                        }
                        else if (j == 1){
                            unsigned long long newP2 = boards[i].p2 | k;
                            newP2 ^= k << 7;
                            unsigned long long newP1 = boards[i].p1 ^ k;
                            newBoards[moveCounts[d][i]+ offset++] = (struct Board){newP1, newP2};
                        }
                        else if (j == 2){
                            unsigned long long newP2 = boards[i].p2 | k;
                            newP2 ^= k << 9;
                            unsigned long long newP1 = boards[i].p1 ^ k;
                            newBoards[moveCounts[d][i]+ offset++] = (struct Board){newP1, newP2};
                        }
                    }
                }
            }
        }

        free(moves);
        numBoards = moveCountsCount[d+1];
        p1Turn = !p1Turn;
        free(boards);
        boards = newBoards;
    }
    *boardsPtr = boards;
}