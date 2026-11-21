#include "functions.h"


struct AiOut Ai (struct Board b, int depth){
    float score = -INFINITY;
    unsigned short bestMoves[64];
    int bestMoveCount = 0;
    struct Moves m = getMovesP1(b);
    for (short i = 0; i < 3; i++){
        unsigned long long *move = (unsigned long long *)&m+i;
        while (*move) {
            float newscore = -INFINITY;
            long long j = 1ULL << __builtin_ctzll(*move);
            *move ^= j;
            if (i == 0){
                unsigned long long newP1 = b.p1 | j;
                newP1 ^= j >> 8;
                struct Board newBoard = {newP1, b.p2};
                newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
            }
            else if (i == 1){
                unsigned long long newP1 = b.p1 | j;
                newP1 ^= j >> 9;
                unsigned long long newP2 = b.p2 ^ j;
                struct Board newBoard = {newP1, newP2};
                newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
            }
            else if (i == 2){
                unsigned long long newP1 = b.p1 | j;
                newP1 ^= j >> 7;
                unsigned long long newP2 = b.p2 ^ j;
                struct Board newBoard = {newP1, newP2};
                newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
            }
            if (newscore == INFINITY)
                return (struct AiOut){newscore, __builtin_ctzll(j) * 3 + i};

            if (score < newscore){
                score = newscore;
                bestMoveCount = 1;
                bestMoves[0] = __builtin_ctzll(j) * 3 + i;
            }
            else if (score == newscore)
                bestMoves[bestMoveCount++] = __builtin_ctzll(j) * 3 + i;

        }
    }

    return (struct AiOut){score, bestMoves[rand() % bestMoveCount]};
}