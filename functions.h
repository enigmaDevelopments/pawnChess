#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structs.h"


static const int16_t weights[] = {1,2,3,10,15,25,50};


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
static inline int16_t getScoreHelper(struct Board b, uint64_t mask, uint64_t mask2){
    int16_t delta = 0;
    uint64_t p1 = b.p1 & mask;
    uint64_t p2 = b.p2 & mask;
    if (!p1 && !p2)
        return 0;
    uint64_t p1Sides = b.p1 & mask2;
    uint64_t p2Sides = b.p2 & mask2;
    int p1Pos = -1;
    int p2Pos = -1;
    if (p1){
        p1Pos = 7 - (__builtin_clzll(p1) / 8);
        int add = weights[p1Pos] * (__builtin_popcountll(p1) - __builtin_popcountll(p2Sides));
        delta += (0 < add) ? add : 0;
    }
    if (p2){
        p2Pos = 7 - (__builtin_ctzll(p2) / 8);
        int add = weights[p2Pos] * (__builtin_popcountll(p2) - __builtin_popcountll(p1Sides));
        delta -= (0 < add) ? add : 0;
    }
    if (p1 && p2 && p1Pos < 7-p2Pos)
        delta += weights[p2Pos] - weights[p1Pos];
    return delta;
}
static inline int16_t getScore(struct Board b){
    int16_t score = 0;
    uint64_t mask = 0x00000000000000FF;
    #pragma unroll
    for (int i = 0; i < 7; i++, mask <<= 8){
        score += weights[i] * __builtin_popcountll(b.p1 & mask);
        score -= weights[6-i] * __builtin_popcountll(b.p2 & mask);
    }
    mask = 0x0101010101010101;
    uint64_t mask2 = 0x0202020202020202;
    score += getScoreHelper(b, mask, mask2);
    mask2 = 0x0505050505050505;
    mask <<= 1;
    #pragma unroll
    for (int i = 1; i < 8; i++, mask <<= 1, mask2 <<= 1)
        score += getScoreHelper(b, mask, mask2);
    return score;
}
static inline bool p1Win(struct Board b){
    return b.p1 & 0xFF00000000000000 || !b.p2;
}
static inline bool p2Win(struct Board b){
    return b.p2 & 0x00000000000000FF || !b.p1;
}


struct AiOut Ai (struct Board b, int depth);

