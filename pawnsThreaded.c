#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


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
struct ThreadedBoards
{
    unsigned long long j;
    short i;
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

struct aiOut Ai (struct Board b, int depth){
    float score = -INFINITY;
    unsigned short bestMoves[64];
    int bestMoveCount = 0;
    struct ThreadedBoards* boards;
    short boardCount = 0;
    {
        struct ThreadedBoards boardsLong[192];
        struct Moves m = getMovesP1(b);
        for (short i = 0; i< 3; i++){
            unsigned long long *move = (unsigned long long *)&m+i;
            while (*move) {
                boardsLong[boardCount].i = i;
                boardsLong[boardCount].j = 1ULL << __builtin_ctzll(*move);
                *move ^= boardsLong[boardCount++].j;
            }
        }
        boards = (struct ThreadedBoards*) malloc(boardCount * sizeof(struct ThreadedBoards));
        for (short i = 0; i < boardCount; i++)
            boards[i] = boardsLong[i];
    }

    bool done = false;

    #pragma omp parallel for
    for (short i = 0; i < boardCount; i++){
        float newscore = -INFINITY;
        if (boards[i].i == 0){
            unsigned long long newP1 = b.p1 | boards[i].j;
            newP1 ^= boards[i].j >> 8;
            struct Board newBoard = {newP1, b.p2};
            newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
        }
        else if (boards[i].i == 1){
            unsigned long long newP1 = b.p1 | boards[i].j;
            newP1 ^= boards[i].j >> 9;
            unsigned long long newP2 = b.p2 ^ boards[i].j;
            struct Board newBoard = {newP1, newP2};
            newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
        }
        else if (boards[i].i == 2){
            unsigned long long newP1 = b.p1 | boards[i].j;
            newP1 ^= boards[i].j >> 7;
            unsigned long long newP2 = b.p2 ^ boards[i].j;
            struct Board newBoard = {newP1, newP2};
            newscore = miniMax(newBoard, -INFINITY, INFINITY, depth - 1, false);
        }
        #pragma omp critical
        {
            if (score <= newscore){

                if (score == newscore)
                    bestMoves[bestMoveCount++] = __builtin_ctzll(boards[i].j) * 3 + boards[i].i;
                else{
                    score = newscore;
                    bestMoveCount = 0;
                    bestMoves[bestMoveCount] = __builtin_ctzll(boards[i].j) * 3 + boards[i].i;
                }
            }
        }
    }
    free(boards);
    return (struct aiOut){score, bestMoves[rand() % bestMoveCount]};
}




int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;
    int depth = atoi(argv[1]);
    struct Board b = {0x000000000000FFFF, 0xFFFF000000000000};
    struct aiOut aiResult = Ai(b, depth);
    printf("Score: %f\n", aiResult.score);
    printf("Move: %hu\n", aiResult.move);
}