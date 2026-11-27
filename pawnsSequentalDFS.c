#include "functions.h"
#include "dfs.h"


struct AiOut Ai (struct Board b, int depth){
    int16_t score = INT16_MIN;
    uint_fast8_t bestMoves[64];
    int bestMoveCount = 0;
    struct Moves m = getMovesP1(b);
    for (short i = 0; i < 3; i++){
        uint64_t *move = (uint64_t *)&m+i;
        while (*move) {
            int16_t newscore = INT16_MIN;
            long long j = 1ULL << __builtin_ctzll(*move);
            *move ^= j;
            if (i == 0){
                uint64_t newP1 = b.p1 | j;
                newP1 ^= j >> 8;
                struct Board newBoard = {newP1, b.p2};
                newscore = miniMax(newBoard, INT16_MIN, INT16_MAX, depth - 1, false);
            }
            else if (i == 1){
                uint64_t newP1 = b.p1 | j;
                newP1 ^= j >> 9;
                uint64_t newP2 = b.p2 ^ j;
                struct Board newBoard = {newP1, newP2};
                newscore = miniMax(newBoard, INT16_MIN, INT16_MAX, depth - 1, false);
            }
            else if (i == 2){
                uint64_t newP1 = b.p1 | j;
                newP1 ^= j >> 7;
                uint64_t newP2 = b.p2 ^ j;
                struct Board newBoard = {newP1, newP2};
                newscore = miniMax(newBoard, INT16_MIN, INT16_MAX, depth - 1, false);
            }

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