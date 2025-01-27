#pragma once
#include "board.h"


class Game {
    [[nodiscard]] static Move Parse_Move(const std::string& move, PieceColour colour);
    [[nodiscard]] static bool Check_Move(const std::string& fromSquare, const std::string& toSquare);

    static Board board;

public:
    static void Play_Game(const std::string& fen);

};