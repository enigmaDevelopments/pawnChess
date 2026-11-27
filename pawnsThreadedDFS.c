#include "functions.h"
#include "dfs.h"
#include <omp.h>


struct AiOut Ai (struct Board b, int depth){
    int16_t score = INT16_MIN;
    uint_fast8_t bestMoves[64];
    int bestMoveCount = 0;
    uint64_t* boards;
    short boardCount = 0;
    short indexes[2] = {0,0}; 
    {
        uint64_t boardsLong[192];
        struct Moves m = getMovesP1(b);
        for (short i = 0; i< 3; i++){
            uint64_t *move = (uint64_t*)&m+i;
            while (*move) {
                boardsLong[boardCount] = 1ULL << __builtin_ctzll(*move);
                *move ^= boardsLong[boardCount++];
            }
            if (i != 2)
                indexes[i] = boardCount;
        }
        boards = (uint64_t*) malloc(boardCount * sizeof(uint64_t));
        for (short i = 0; i < boardCount; i++)
            boards[i] = boardsLong[i];
    }

    #pragma omp parallel for schedule(static,1)
    for (short i = 0; i < boardCount; i++){
        int16_t newscore = INT16_MIN;
        short moveType;
        if (i < indexes[0]){
            moveType = 0;
            uint64_t newP1 = b.p1 | boards[i];
            newP1 ^= boards[i] >> 8;
            struct Board newBoard = {newP1, b.p2};
            newscore = miniMax(newBoard, INT16_MIN, INT16_MAX, depth - 1, false);
        }
        else if (i < indexes[1]){
            moveType = 1;
            uint64_t newP1 = b.p1 | boards[i];
            newP1 ^= boards[i] >> 9;
            uint64_t newP2 = b.p2 ^ boards[i];
            struct Board newBoard = {newP1, newP2};
            newscore = miniMax(newBoard, INT16_MIN, INT16_MAX, depth - 1, false);
        }
        else {
            moveType = 2;
            uint64_t newP1 = b.p1 | boards[i];
            newP1 ^= boards[i] >> 7;
            uint64_t newP2 = b.p2 ^ boards[i];
            struct Board newBoard = {newP1, newP2};
            newscore = miniMax(newBoard, INT16_MIN, INT16_MAX, depth - 1, false);
        }
        
        #pragma omp critical
        {
            if (score == newscore)
                bestMoves[bestMoveCount++] = __builtin_ctzll(boards[i]) * 3 + moveType;
            else if (score < newscore){
                score = newscore;
                bestMoveCount = 1;
                bestMoves[0] = __builtin_ctzll(boards[i]) * 3 + moveType;
            }
        }
    }
    free(boards);
    return (struct AiOut){score, bestMoves[rand() % bestMoveCount]};
}


