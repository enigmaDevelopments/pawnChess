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
struct AiOut
{
    float score;
    unsigned short move;
};

static inline struct Moves getMovesP1(struct Board b){
    struct Moves m;
    m.strightMoves = (b.p1 << 8) & ~b.p1 & ~b.p2;
    m.leftMoves =  (b.p1 << 9) & b.p2 & ~b.p1 & 0xFEFEFEFEFEFEFEFE;
    m.rightMoves = (b.p1 << 7) & b.p2 & ~b.p1 & 0x7F7F7F7F7F7F7F7F;
    return m;
}
static inline struct Moves getMovesP2(struct Board b){
    struct Moves m;
    m.strightMoves = (b.p2 >> 8) & ~b.p2 & ~b.p1;
    m.leftMoves = (b.p2 >> 7) & b.p1 & ~b.p2 & 0xFEFEFEFEFEFEFEFE;
    m.rightMoves = (b.p2 >> 9) & b.p1 & ~b.p2 & 0x7F7F7F7F7F7F7F7F;
    return m;
}
static inline float getScore(struct Board b){
    float weights[] = {1,2,3,10,15,25,50};
    float score = 0;
    unsigned long long mask = 0x00000000000000FF;
    for (int i = 0; i < 7; i++, mask <<= 8){
        score += weights[i] * __builtin_popcountll(b.p1 & mask);
        score -= weights[6-i] * __builtin_popcountll(b.p2 & mask);
    }

    return score;
}
float miniMax (struct Board b, float alpha, float beta, int depth, bool p1Turn);
struct AiOut Ai (struct Board b, int depth);

