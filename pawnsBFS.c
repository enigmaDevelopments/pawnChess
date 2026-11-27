#include "functions.h"
#include "bfs.h"

struct AiOut Ai (struct Board b, int depth){
    struct Board* boards = malloc(sizeof(struct Board));
    int** moveCounts = malloc(sizeof(int*) * depth);
    int* moveCountsCount = malloc(sizeof(int) * (depth+1));
    boards[0] = b;
    moveCountsCount[0] = 1;
    forwardPass(&boards, moveCounts, moveCountsCount, 1,  depth, true);

    int16_t* scores = scorePass(boards, moveCountsCount[depth]);
    int16_t bestScore = backwardPass(&scores, moveCounts, moveCountsCount, depth, !((depth) & 1));

    int* bestMoves = malloc(sizeof(int) * moveCountsCount[1]);
    int bestMoveCount = 0;
    for (int i = 0; i < moveCountsCount[1]; i++)
        if(bestScore == scores[i])
           bestMoves[bestMoveCount++] = i;

    int chosen = bestMoves[rand() % bestMoveCount];
    struct Moves m = getMovesP1(b);
    for (short i = 0; i < 3; i++){
        uint64_t *move = (uint64_t *)&m + i;
        while (*move){
            int pos = __builtin_ctzll(*move);
            long long j = 1ULL << pos;
            *move ^= j;
            if (chosen-- == 0) {
                struct AiOut out = {scores[0],pos * 3 + i};
                free(boards);
                free(moveCountsCount);
                free(bestMoves);
                free(scores);
                free(moveCounts);
                return out;
            }
        }
    }
}