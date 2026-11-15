#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>


struct Moves{
    unsigned long long strightMoves;
    unsigned long long leftMoves;
    unsigned long long rightMoves;
};
struct Board{
    unsigned long long p1;
    unsigned long long p2;
};
struct aiOut
{
    float score;
    unsigned short move;
};



struct Moves getMovesP1(struct Board b)
{
    struct Moves m;
    m.strightMoves = (b.p1 << 8) & ~b.p1 & ~b.p2;
    m.leftMoves =  (b.p1 << 9) & b.p2 & ~b.p1 & 0xFEFEFEFEFEFEFEFE;
    m.rightMoves = (b.p1 << 7) & b.p2 & ~b.p1 & 0x7F7F7F7F7F7F7F7F;
    return m;
}
struct Moves getMovesP2(struct Board b)
{
    struct Moves m;
    m.strightMoves = (b.p2 >> 8) & ~b.p2 & ~b.p1;
    m.leftMoves = (b.p2 >> 7) & b.p1 & ~b.p2 & 0xFEFEFEFEFEFEFEFE;
    m.rightMoves = (b.p2 >> 9) & b.p1 & ~b.p2 & 0x7F7F7F7F7F7F7F7F;
    return m;
}


float getScore(struct Board b){
    float weights[] = {1,2,3,10,15,25,50,0};
    float score = 0;
    unsigned long long mask = 0x00000000000000FF;
    for (int i = 0; i < 7; i++, mask <<= 8){
        score += weights[i] * __builtin_popcountll(b.p1 & mask);
        score -= weights[6-i] * __builtin_popcountll(b.p2 & mask);
    }

    return score;
}


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
        for (unsigned long long i = 1, count = 0; count < 64; i <<= 1, count++){
            for (int j = 0; j < 3; j ++){
                float newscore = -INFINITY;
                if (j == 0 && m.strightMoves & i){
                    unsigned long long newP1 = b.p1 | i;
                    newP1 ^= i >> 8;
                    struct Board newBoard = {newP1, b.p2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, false);
                }
                else if (j == 1 && m.leftMoves & i){
                    unsigned long long newP1 = b.p1 | i;
                    newP1 ^= i >> 9;
                    unsigned long long newP2 = b.p2 ^ i;
                    struct Board newBoard = {newP1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, false);
                }
                else if (j == 2 && m.rightMoves & i){
                    unsigned long long newP1 = b.p1 | i;
                    newP1 ^= i >> 7;
                    unsigned long long newP2 = b.p2 ^ i;
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
        for (unsigned long long i = 1, count = 0; count < 64; i <<= 1, count++){
            for (int j = 0; j < 3; j ++){
                float newscore = INFINITY;
                if (j == 0 && m.strightMoves & i){
                    unsigned long long newP2 = b.p2 | i;
                    newP2 ^= i << 8;
                    struct Board newBoard = {b.p1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, true);
                }
                else if (j == 1 && m.leftMoves & i){
                    unsigned long long newP2 = b.p2 | i;
                    newP2 ^= i << 7;
                    unsigned long long newP1 = b.p1 ^ i;
                    struct Board newBoard = {newP1, newP2};
                    newscore = miniMax(newBoard, alpha, beta, depth - 1, true);
                }
                else if (j == 2 && m.rightMoves & i){
                    unsigned long long newP2 = b.p2 | i;
                    newP2 ^= i << 9;
                    unsigned long long newP1 = b.p1 ^ i;
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

struct aiOut Ai (struct Board b, int depth){
    float score = -INFINITY;
    unsigned short bestMoves[64];
    int bestMoveCount = 0;
    struct Moves m = getMovesP1(b);
    for (unsigned long long i = 1, count = 0; count < 64; i <<= 1, count++){
        for (int j = 0; j < 3; j++){
            float newscore = -INFINITY;
            if (j == 0 && m.strightMoves & i){
                unsigned long long newP1 = b.p1 | i;
                newP1 ^= i >> 8;
                struct Board newBoard = {newP1, b.p2};
                newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
            }
            else if (j == 1 && m.leftMoves & i){
                unsigned long long newP1 = b.p1 | i;
                newP1 ^= i >> 9;
                unsigned long long newP2 = b.p2 ^ i;
                struct Board newBoard = {newP1, newP2};
                newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
            }
            else if (j == 2 && m.rightMoves & i){
                unsigned long long newP1 = b.p1 | i;
                newP1 ^= i >> 7;
                unsigned long long newP2 = b.p2 ^ i;
                struct Board newBoard = {newP1, newP2};
                newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
            }
            if (newscore == INFINITY)
                return (struct aiOut){newscore, count * 3 + j};

            if (score < newscore){
                score = newscore;
                bestMoveCount = 0;
                bestMoves[bestMoveCount] = count * 3 + j;
            }
            else if (score == newscore)
                bestMoves[bestMoveCount++] = count * 3 + j;

        }
    }

    return (struct aiOut){score, bestMoves[rand() % bestMoveCount]};
}




int main(int argc, char *argv[])
{
    if (argc < 1)
        return 1;
    int depth = atoi(argv[1]);
    struct Board b = {0x000000000000FFFF, 0xFFFF000000000000};
    struct aiOut aiResult = Ai(b, depth);
    printf("Score: %f\n", aiResult.score);
    printf("Move: %hu\n", aiResult.move);
}