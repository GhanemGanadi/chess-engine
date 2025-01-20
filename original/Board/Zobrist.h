#pragma once
#include <random>
#include <cstdint>
#include "Board.h"

class Zobrist {
    static const int PIECE_TYPES = 12;
    static const int SQUARES = 64;

    U64 Piece_Square[PIECE_TYPES][SQUARES];
    U64 Side_To_Move;
    U64 Castling[16];
    U64 En_Passant_File[8];

    static int Get_Piece_Index(PieceColour colour, PieceType piece) ;
    int Get_Castling_Index(const Board& board) const;

public:
    void Initialise();
    [[nodiscard]] U64 Calculate_Hash_Initial_Position() const;
    U64 Zobrist_Make_Move(const Move& move, U64 hash) const;
    U64 Calculate_Hash(const Board& board) const;
};