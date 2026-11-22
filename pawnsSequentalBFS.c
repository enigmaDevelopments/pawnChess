#include "functions.h"
struct AiOut Ai (struct Board b, int depth){
    struct Board* boards = malloc(sizeof(struct Board));
    struct Moves* moves;
    int** moveCounts = malloc(sizeof(int*) * depth);
    moveCounts[0] = malloc(sizeof(int) * 2);
    int* moveCountsCount = malloc(sizeof(int) * (depth+1));
    float** scores = malloc(sizeof(float*) * (depth+1));
    int numBoards = 1;
    boards[0] = b;
    moveCountsCount[0] = 1;
    bool p1Turn = true;

    for (int d = 0; d < depth; d++){
        moves = (struct Moves*) malloc(sizeof(struct Moves) * numBoards);
        scores[d] = malloc(sizeof(float) * numBoards);
        moveCounts[d] = malloc(sizeof(int) * (numBoards+1));
        moveCounts[d][0] = 0;
        
        if (p1Turn){
            for (int i = 0; i < numBoards; i++){
                moves[i] = getMovesP1(boards[i]);
                if (boards[i].p1 & 0x00000000000000FF || !boards[i].p2) 
                    scores[d][i] = INFINITY;
                else if (!(moves[i].strightMoves | moves[i].leftMoves | moves[i].rightMoves))
                    scores[d][i] = 0;
                else
                    scores[d][i] = -INFINITY;
            }
        }
        else{
            for (int i = 0; i < numBoards; i++){
                moves[i] = getMovesP2(boards[i]);
                if (boards[i].p2 & 0x00000000000000FF || !boards[i].p1) 
                    scores[d][i] = -INFINITY;
                else if (!(moves[i].strightMoves | moves[i].leftMoves | moves[i].rightMoves))
                    scores[d][i] = 0;
                else
                    scores[d][i] = INFINITY;
            }
        }

        {
            bool localP1turn = !p1Turn;
            for (int i = d-1; 0 <= i; i--){
                for (int j = 0;  j < moveCountsCount[i]; j++){
                    float localMinMax = scores[i][j];
                    for (int k = moveCounts[i][j]; k < moveCounts[i][j+1]; k++){
                        if (localP1turn && localMinMax < scores[i+1][k])
                            localMinMax = scores[i+1][k];
                        else if (!localP1turn && scores[i+1][k] < localMinMax)
                            localMinMax = scores[i+1][k];
                    }
                    scores[i][j] = localMinMax;
                    if ((localP1turn && localMinMax == INFINITY) || (!localP1turn && localMinMax == -INFINITY)){
                        int left = moveCounts[i][j];
                        int right = moveCounts[i][j];
                        for (int k = 0; k< d; k++){
                            left = moveCounts[k][left];
                            right = moveCounts[k][right];
                        }
                        for (int k = left; k < right; k++)
                            moves[k] = (struct Moves){0,0,0};
                    }
                }
                localP1turn = !localP1turn;
            }
        }

        for (int i = 0; i < numBoards; i++){
            moveCounts[d][i+1] = __builtin_popcountll(moves[i].strightMoves) + __builtin_popcountll(moves[i].leftMoves) + __builtin_popcountll(moves[i].rightMoves);
            if (moveCounts[d][i+1] == 0)
                moveCounts[d][i+1] = 1;
            moveCounts[d][i+1] += moveCounts[d][i];
        }

        moveCountsCount[d+1] = moveCounts[d][numBoards];
        struct Board* newBoards = malloc(sizeof(struct Board) * moveCountsCount[d+1]);

        if (p1Turn){
            for (int i = 0; i < numBoards; i++){
                if (moves[i].leftMoves == 0 && moves[i].rightMoves == 0 && moves[i].strightMoves == 0){
                    newBoards[moveCounts[d][i]] = (struct Board){0x10000000ULL,0x1000000000ULL};
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
                if (offset == 0 )
                    newBoards[moveCounts[d][i]] = (struct Board) {0,0};
            }
        }
        else{
            for (int i = 0; i < numBoards; i++){
                if (moves[i].leftMoves == 0 && moves[i].rightMoves == 0 && moves[i].strightMoves == 0){
                    newBoards[moveCounts[d][i]] = (struct Board){0x10000000ULL,0x1000000000ULL};
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
    for (int i = 0; i < numBoards; i++)
        scores[depth][i] = getScore(boards[i]);
    
    for (int i = depth-1; 0 <= i; i--){
        p1Turn = !p1Turn;
        for (int j = 0;  j < moveCountsCount[i]; j++){
            float localMinMax = scores[i][j];
            for (int k = moveCounts[i][j]; k < moveCounts[i][j+1]; k++){
                if (p1Turn && localMinMax < scores[i+1][k])
                    localMinMax = scores[i+1][k];
                else if (!p1Turn && scores[i+1][k] < localMinMax)
                    localMinMax = scores[i+1][k];
            }
            scores[i][j] = localMinMax;
        }
    }

    int* bestMoves = malloc(sizeof(int) * moveCountsCount[1]);
    int bestMoveCount = 0;
    for (int i = 0; i < moveCountsCount[1]; i++)
        if(scores[0][0] == scores[1][i])
           bestMoves[bestMoveCount++] = i;
    int chosen = bestMoves[rand() % bestMoveCount];
    struct Moves m = getMovesP1(b);
    for (short i = 0; i < 3; i++){
        unsigned long long *move = (unsigned long long *)&m + i;
        while (*move){
            int pos = __builtin_ctzll(*move);
            long long j = 1ULL << pos;
            *move ^= j;
            if (chosen-- == 0) {
                struct AiOut out = {scores[0][0],pos * 3 + i};
                free(boards);
                free(moveCountsCount);
                free(bestMoves);
                for(int k = 0; k < depth; k++){
                    free(moveCounts[k]);
                    free(scores[k]);
                }
                free(moveCounts);
                free(scores);
                return out;
            }
        }
    }
}