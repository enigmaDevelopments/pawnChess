#include "functions.h"
#include "bfs.h"
#ifdef _OPENMP
#include <omp.h>
#endif

int16_t backwardPass(int16_t** scoresPtr, int** moveCounts, int* moveCountsCount, int depth, bool p1Turn){
    int16_t* lastScores = *scoresPtr;
    int16_t* scores;

    for (int i = depth-1; 0 <= i; i--){
        p1Turn = !p1Turn;
        #ifdef _OPENMP
            scores = malloc(sizeof(int16_t) * moveCountsCount[i]);
        #else
            scores = (int16_t*) moveCounts[i];
        #endif
        if (p1Turn){
            #pragma omp parallel for if (3 < moveCountsCount[i])
            for (int j = 0;  j < moveCountsCount[i]; j++){
                int16_t localMax= INT16_MIN;
                #pragma omp simd reduction(max:localMax)
                for (int k = moveCounts[i][j]; k < moveCounts[i][j+1]; k++){
                    if (localMax < lastScores[k])
                        localMax = lastScores[k];
                }
                scores[j] = localMax;
            }
        }
        else{
            #pragma omp parallel for if (3 < moveCountsCount[i])
            for (int j = 0;  j < moveCountsCount[i]; j++){
                int16_t localMin = INT16_MAX;
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
    int16_t output = scores[0];
    free(scores);
    return output;
}