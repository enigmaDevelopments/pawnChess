#include "structs.h"

void forwardPass(struct Board** boardsPtr,int** moveCounts,int* moveCountsCount,int numBoards, int depth, bool p1Turn);
float* scorePass(struct Board* boards, int numBoards);
float backwardPass(float** scoresPtr, int** moveCounts, int* moveCountsCount, int depth, bool p1Turn);
