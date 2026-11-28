#include "functions.h"
#include "bfs.h"
#ifdef _OPENMP
#include <omp.h>
#endif

int16_t* scorePass(struct Board* boards, int numBoards){
    int16_t* scores = malloc(sizeof(int16_t) * numBoards);
    #pragma omp parallel for simd schedule(static)
    for (int i = 0; i < numBoards; i++){
        if (p2Win(boards[i]))
            scores[i] = INT16_MIN;
        else if (p1Win(boards[i]))
            scores[i] = INT16_MAX;
        else
            scores[i] = getScore(boards[i]);
    }
    return scores;
}