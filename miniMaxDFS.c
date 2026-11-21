#include "functions.h"


float miniMax (struct Board b, float alpha, float beta, int depth, bool p1Turn){
    float score;
    if (p1Turn) {
        if (b.p2 & 0x00000000000000FF || !b.p1)
            return -INFINITY;
        if (depth == 0) 
            return getScore(b);
        score = -INFINITY;
        struct Moves m = getMovesP1(b);
        if (!(m.strightMoves | m.leftMoves | m.rightMoves))
            return 0;
        for (short i = 0; i < 3; i++){
            unsigned long long *move = (unsigned long long *)&m+i;
            while (*move){
                float newscore = -INFINITY;
                long long j = 1ULL << __builtin_ctzll(*move);
                *move ^= j;
                if (i == 0){
                    unsigned long long newP1 = b.p1 | j;
                    newP1 ^= j >> 8;
                    struct Board newBoard = {newP1, b.p2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, false);
                }
                else if (i == 1){
                    unsigned long long newP1 = b.p1 | j;
                    newP1 ^= j >> 9;
                    unsigned long long newP2 = b.p2 ^ j;
                    struct Board newBoard = {newP1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, false);
                }
                else if (i == 2){
                    unsigned long long newP1 = b.p1 | j;
                    newP1 ^= j >> 7;
                    unsigned long long newP2 = b.p2 ^ j;
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
        if (b.p1 & 0xFF00000000000000 || !b.p2) 
            return INFINITY;
        if (depth == 0) 
            return getScore(b);
        score = INFINITY;
        struct Moves m = getMovesP2(b);
        if (!(m.strightMoves | m.leftMoves | m.rightMoves))
            return 0;
        for (int i = 0; i < 3; i++){
            unsigned long long *move = (unsigned long long *)&m+i;
            while (*move) {
                float newscore = INFINITY;
                long long j = 1ULL << __builtin_ctzll(*move);
                *move ^= j;
                if (i == 0){
                    unsigned long long newP2 = b.p2 | j;
                    newP2 ^= j << 8;
                    struct Board newBoard = {b.p1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, true);
                }
                else if (i == 1){
                    unsigned long long newP2 = b.p2 | j;
                    newP2 ^= j << 7;
                    unsigned long long newP1 = b.p1 ^ j;
                    struct Board newBoard = {newP1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, true);
                }
                else if (i == 2){
                    unsigned long long newP2 = b.p2 | j;
                    newP2 ^= j << 9;
                    unsigned long long newP1 = b.p1 ^ j;
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