#include "functions.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;
    int depth = atoi(argv[1]);
    struct Board b = {0x000000000000FFFF, 0xFFFF000000000000};
    struct AiOut aiResult = Ai(b, depth);
    printf("Score: %hu\n", aiResult.score);
    printf("Move: %hhu\n", aiResult.move);
}