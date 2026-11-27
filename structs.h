#ifndef STRUCTS
#define STRUCTS
struct Moves{
    unsigned long long strightMoves;
    unsigned long long leftMoves;
    unsigned long long rightMoves;
};
struct Board{
    unsigned long long p1;
    unsigned long long p2;
};
struct AiOut
{
    float score;
    unsigned short move;
};

#endif