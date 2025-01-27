#pragma once
#include "../Move_Generator.h"

class Perft {
public:

    int Run_Perft(int depth, Board& board);
    void Perft_Divide(int depth, Board& board);
    void Perft_Divide_Debugging(int depth, Board& board);
};
