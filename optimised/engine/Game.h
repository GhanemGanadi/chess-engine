#pragma once
#include "board.h"


class Game {

public:
    [[nodiscard]] static Move Parse_Move(const std::string& move, PieceColour colour, const Board& board);
    [[nodiscard]] static bool Check_Move(const std::string& fromSquare, const std::string& toSquare);
    static void Play_Game(const std::string& fen);

};