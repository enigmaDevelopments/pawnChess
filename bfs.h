#include "structs.h"

void forwardPass(struct Board** boardsPtr,int** moveCounts,int* moveCountsCount,int numBoards, int depth, bool p1Turn);
int16_t* scorePass(struct Board* boards, int numBoards);
int16_t backwardPass(int16_t** scoresPtr, int** moveCounts, int* moveCountsCount, int depth, bool p1Turn);
