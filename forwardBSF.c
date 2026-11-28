#include "functions.h"
#include "bfs.h"
#ifdef _OPENMP
#include <omp.h>
#endif


void forwardPass(struct Board** boardsPtr,int** moveCounts,int* moveCountsCount,int numBoards,int depth, bool p1Turn){
    struct Board* boards = *boardsPtr;
    struct Moves* moves;
    moveCounts[0] = malloc(sizeof(int) * 2);

    for (int d = 0; d < depth; d++){
        moves = (struct Moves*) malloc(sizeof(struct Moves) * numBoards);
        moveCounts[d] = malloc(sizeof(int) * (numBoards+1));
        {
            moveCounts[d][0] = 0;
            int prefix = 0;
            int count;
            if (p1Turn){
                #pragma omp parallel for simd reduction(inscan, +:prefix) private(count) if (numBoards != 1)
                for (int i = 0; i < numBoards; i++){
                    if (p1Win(boards[i]) || p2Win(boards[i]))
                        moves[i] = (struct Moves) {0, 0, 1};
                    else
                        moves[i] = getMovesP1(boards[i]);
                    count = __builtin_popcountll(moves[i].strightMoves) + __builtin_popcountll(moves[i].leftMoves) + __builtin_popcountll(moves[i].rightMoves);
                    if (count == 0)
                        count = 1;
                    prefix += count;
                    #pragma omp scan inclusive(prefix)
                    moveCounts[d][i+1] = prefix;
                }
            }
            else{
                #pragma omp parallel for simd reduction(inscan, +:prefix) private(count) if (numBoards != 1)
                for (int i = 0; i < numBoards; i++){
                    if (p1Win(boards[i]) || p2Win(boards[i]))
                        moves[i] = (struct Moves) {0, 0, 0x8000000000000000ULL};
                    else
                        moves[i] = getMovesP2(boards[i]);
                    count = __builtin_popcountll(moves[i].strightMoves) + __builtin_popcountll(moves[i].leftMoves) + __builtin_popcountll(moves[i].rightMoves);
                    if (count == 0)
                        count = 1;
                    prefix += count;
                    #pragma omp scan inclusive(prefix)
                    moveCounts[d][i+1] = prefix;   
                }
            }
        }

        moveCountsCount[d+1] = moveCounts[d][numBoards];
        struct Board* newBoards = malloc(sizeof(struct Board) * moveCountsCount[d+1]);
        #ifdef _OPENMP
            if (numBoards < 1024 * (omp_get_num_threads()+2))
                omp_set_schedule(omp_sched_static, 0);
            else
               omp_set_schedule(omp_sched_dynamic, 1024);
        #endif
        if (p1Turn){
            #pragma omp parallel for if (numBoards != 1) schedule(runtime)
            for (int i = 0; i < numBoards; i++){
                {
                    uint64_t con = moves[i].leftMoves | moves[i].strightMoves | moves[i].rightMoves;
                    if (!con){
                        newBoards[moveCounts[d][i]] = (struct Board){0x10000000ULL,0x1000000000ULL};
                        continue;
                    }
                    else if (con== 1){
                        newBoards[moveCounts[d][i]] = boards[i];
                        continue;
                    }
                }
  
                int offset = 0;
                #pragma unroll
                for (short j = 0; j < 3; j++){
                    uint64_t *move = (uint64_t *)&moves[i] + j;
                    while (*move){
                        long long k = 1ULL << __builtin_ctzll(*move);
                        *move ^= k;
                        if (j == 0){
                            uint64_t newP1 = boards[i].p1 | k;
                            newP1 ^= k >> 8;
                            newBoards[moveCounts[d][i] + offset++] = (struct Board){newP1, boards[i].p2};
                        }
                        else if (j == 1){
                            uint64_t newP1 = boards[i].p1 | k;
                            newP1 ^= k >> 9;
                            uint64_t newP2 = boards[i].p2 ^ k;
                            newBoards[moveCounts[d][i] + offset++] = (struct Board){newP1, newP2};
                        }
                        else if (j == 2){
                            uint64_t newP1 = boards[i].p1 | k;
                            newP1 ^= k >> 7;
                            uint64_t newP2 = boards[i].p2 ^ k;
                            newBoards[moveCounts[d][i] + offset++] = (struct Board){newP1, newP2};
                        }
                    }
                }
            }
        }
        else{
            #pragma omp parallel for if (numBoards != 1) schedule(runtime)
            for (int i = 0; i < numBoards; i++){
                {
                    uint64_t con = moves[i].leftMoves | moves[i].strightMoves | moves[i].rightMoves;
                    if (!con){
                        newBoards[moveCounts[d][i]] = (struct Board){0x10000000ULL,0x1000000000ULL};
                        continue;
                    }
                    else if (con == 0x8000000000000000ULL){
                        newBoards[moveCounts[d][i]] = boards[i];
                        continue;
                    }
                }
                int offset = 0;
                #pragma unroll
                for (short j = 0; j < 3; j++){
                    uint64_t *move = (uint64_t *)&moves[i] + j;
                    while (*move){
                        long long k = 1ULL << __builtin_ctzll(*move);
                        *move ^= k;
                        if (j == 0){
                            uint64_t newP2 = boards[i].p2 | k;
                            newP2 ^= k << 8;
                            newBoards[moveCounts[d][i]+ offset++] = (struct Board){boards[i].p1, newP2};
                        }
                        else if (j == 1){
                            uint64_t newP2 = boards[i].p2 | k;
                            newP2 ^= k << 7;
                            uint64_t newP1 = boards[i].p1 ^ k;
                            newBoards[moveCounts[d][i]+ offset++] = (struct Board){newP1, newP2};
                        }
                        else if (j == 2){
                            uint64_t newP2 = boards[i].p2 | k;
                            newP2 ^= k << 9;
                            uint64_t newP1 = boards[i].p1 ^ k;
                            newBoards[moveCounts[d][i]+ offset++] = (struct Board){newP1, newP2};
                        }
                    }
                }
            }
        }

        free(moves);
        numBoards = moveCountsCount[d+1];
        p1Turn = !p1Turn;
        free(boards);
        boards = newBoards;
    }
    *boardsPtr = boards;
}