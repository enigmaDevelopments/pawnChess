#include "functions.h"
#include "bfs.h"
#ifdef _OPENMP
#include <omp.h>
#endif

float* scorePass(struct Board* boards, int numBoards){
    float* scores = malloc(sizeof(float) * numBoards);
    #pragma omp parallel for simd
    for (int i = 0; i < numBoards; i++){
        if (p2Win(boards[i]))
            scores[i] = -INFINITY;
        else if (p1Win(boards[i]))
            scores[i] = INFINITY;
        else
            scores[i] = getScore(boards[i]);
    }
    return scores;
}