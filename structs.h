#ifndef STRUCTS
#define STRUCTS
#include <stdint.h>
#include <stdbool.h>
struct Moves{
    uint64_t strightMoves;
    uint64_t leftMoves;
    uint64_t rightMoves;
};
struct Board{
    uint64_t p1;
    uint64_t p2;
};
struct AiOut
{
    uint_fast8_t score;
    int8_t move;
};

#endif