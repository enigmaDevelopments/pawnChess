#include "functions.h"
#include "bfs.h"
#ifdef _OPENMP
#include <omp.h>
#endif

float* scorePass(struct Board* boards, int numBoards){
    float* scores = malloc(sizeof(float) * numBoards);
    #pragma omp parallel for simd
    for (int i = 0; i < numBoards; i++){
        if (boards[i].p2 & 0x00000000000000FF || !boards[i].p1)
            scores[i] = -INFINITY;
        else if (boards[i].p1 & 0xFF00000000000000 || !boards[i].p2)
            scores[i] = INFINITY;
        else
            scores[i] = getScore(boards[i]);
    }
    return scores;
}