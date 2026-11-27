#include "functions.h"
#include "dfs.h"


int16_t miniMax (struct Board b, int16_t alpha, int16_t beta, int depth, bool p1Turn){
    int16_t score;
    if (p1Turn) {
        if (p2Win(b))
            return INT16_MIN;
        if (depth == 0) 
            return getScore(b);
        score = INT16_MIN;
        struct Moves m = getMovesP1(b);
        if (!(m.strightMoves | m.leftMoves | m.rightMoves))
            return 0;
        for (short i = 0; i < 3; i++){
            uint64_t *move = (uint64_t *)&m+i;
            while (*move){
                int16_t newscore = INT16_MIN;
                long long j = 1ULL << __builtin_ctzll(*move);
                *move ^= j;
                if (i == 0){
                    uint64_t newP1 = b.p1 | j;
                    newP1 ^= j >> 8;
                    struct Board newBoard = {newP1, b.p2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, false);
                }
                else if (i == 1){
                    uint64_t newP1 = b.p1 | j;
                    newP1 ^= j >> 9;
                    uint64_t newP2 = b.p2 ^ j;
                    struct Board newBoard = {newP1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, false);
                }
                else if (i == 2){
                    uint64_t newP1 = b.p1 | j;
                    newP1 ^= j >> 7;
                    uint64_t newP2 = b.p2 ^ j;
                    struct Board newBoard = {newP1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, false);
                
                }
                if (score < newscore){
                    score = newscore;
                    if (alpha < score){
                        alpha = score;
                        if (beta <= alpha)
                            return score;
                    }
                }
            }
        }
    } else {
        if (p2Win(b)) 
            return INT16_MAX;
        if (depth == 0) 
            return getScore(b);
        score = INT16_MAX;
        struct Moves m = getMovesP2(b);
        if (!(m.strightMoves | m.leftMoves | m.rightMoves))
            return 0;
        for (int i = 0; i < 3; i++){
            uint64_t *move = (uint64_t *)&m+i;
            while (*move) {
                int16_t newscore = INT16_MAX;
                long long j = 1ULL << __builtin_ctzll(*move);
                *move ^= j;
                if (i == 0){
                    uint64_t newP2 = b.p2 | j;
                    newP2 ^= j << 8;
                    struct Board newBoard = {b.p1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, true);
                }
                else if (i == 1){
                    uint64_t newP2 = b.p2 | j;
                    newP2 ^= j << 7;
                    uint64_t newP1 = b.p1 ^ j;
                    struct Board newBoard = {newP1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, true);
                }
                else if (i == 2){
                    uint64_t newP2 = b.p2 | j;
                    newP2 ^= j << 9;
                    uint64_t newP1 = b.p1 ^ j;
                    struct Board newBoard = {newP1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, true);
                }
                if (newscore < score){
                    score = newscore;
                    if (score < beta){
                        beta = score;
                        if (beta <= alpha)
                            return score;
                    }
                }
            }  
        }
    }
    return score;
}