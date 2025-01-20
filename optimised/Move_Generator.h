#pragma once

#include "core/board.h"

class Move_Generator {

    public:
    [[nodiscard]] static BB Get_Pseudo_Legal_Moves(int square, PieceType piece,
                                                    PieceColour colour, const Board& board);


    [[nodiscard]] static BB Get_Legal_Moves(int square, PieceType piece, PieceColour colour, const Board& board);
    [[nodiscard]] static std::vector<Move> Generate_All_Moves(PieceColour colour, const Board& board);
    [[nodiscard]] static BB Get_Pinned_Pieces(int king_square, PieceColour colour, const Board& board);
    [[nodiscard]] static BB Get_Pin_Line(int pinned_square, int king_square, const Board& board);
};