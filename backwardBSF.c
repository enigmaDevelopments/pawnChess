#include "functions.h"
#include "bfs.h"
#ifdef _OPENMP
#include <omp.h>
#endif

float backwardPass(float** scoresPtr, int** moveCounts, int* moveCountsCount, int depth, bool p1Turn){
    float* lastScores = *scoresPtr;
    float* scores;

    for (int i = depth-1; 0 <= i; i--){
        p1Turn = !p1Turn;
        #ifdef _OPENMP
            scores = malloc(sizeof(float) * moveCountsCount[i]);
        #else
            scores = (float*) moveCounts[i];
        #endif
        if (p1Turn){
            #pragma omp parallel for
            for (int j = 0;  j < moveCountsCount[i]; j++){
                float localMax= -INFINITY;
                #pragma omp simd reduction(max:localMax)
                for (int k = moveCounts[i][j]; k < moveCounts[i][j+1]; k++){
                    if (localMax < lastScores[k])
                        localMax = lastScores[k];
                }
                scores[j] = localMax;
            }
        }
        else{
            #pragma omp parallel for
            for (int j = 0;  j < moveCountsCount[i]; j++){
                float localMin = INFINITY;
                #pragma omp simd reduction(min:localMin)
                for (int k = moveCounts[i][j]; k < moveCounts[i][j+1]; k++){
                    if (lastScores[k] < localMin)
                        localMin = lastScores[k];
                }
                scores[j] = localMin;
            }
        }
        #ifdef _OPENMP
            free(moveCounts[i]);
        #endif
        if (i != 0){
            free(lastScores);
            lastScores = scores;
        }
    }
    *scoresPtr = lastScores;
    float output = scores[0];
    free(scores);
    return output;
}