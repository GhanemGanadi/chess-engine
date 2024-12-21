#pragma once
#include <array>
#include "../Board/Board.h"
#include "../Moves/Move_Generation.h"

namespace Engine{


class King_Evaluation {
    MoveGeneration moveGen;
    static constexpr std::array<int, 6> ATTACK_WEIGHTS = { 20, 20, 40, 80 };
    static constexpr std::array<int, 7> NO_OF_ATTACKERS_MULTIPLIER = { 0, 50, 75, 88, 94, 97, 99 };


public:
    int Evaluate_King_Attack_Zone(const Board& board, PieceColour colour);
    U64 Get_King_Attack_Zone(const Board& board, PieceColour colour);

};

}