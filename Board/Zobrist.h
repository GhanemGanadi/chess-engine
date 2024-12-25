#pragma once
#include <random>
#include <cstdint>
#include "Board.h"

class Zobrist {
private:
    static const int PIECE_TYPES = 12;
    static const int SQUARES = 64;

    U64 Piece_Square[PIECE_TYPES][SQUARES];
    U64 Side_To_Move;
    U64 Castling[16];
    U64 En_Passant_File[8];

    int Get_Piece_Index(PieceColour colour, PieceType piece) const;
    int Get_Castling_Index(const Board& board) const;

public:
    void Initialise();
    U64 Calculate_Hash(const Board& board) const;
};