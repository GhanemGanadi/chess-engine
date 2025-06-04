#pragma once
#include "core/board.h"


class Attack_Tables {
    static BB Bishop_Moves[64][512];
    static BB Rook_Moves[64][4096];
    static BB Bishop_Masks[64];
    static BB Rook_Masks[64];

    static int Relevant_Rook_Squares[64];
    static int Bishop_Relevant_Squares[64];

    static std::array<BB, 64 * 4096> Rook_Magic_Numbers;
    static std::array<BB, 64 * 512>  Bishop_Magic_Numbers;

    static std::array<BB, 64> Knight_Moves;
    static std::array<BB, 64> King_Moves;

    static std::array<std::array<BB, 64>, 2> Pawn_Moves;
    static std::array<std::array<BB, 64>, 2> Pawn_Attacks;

    static BB Between_Squares[64][64];

    static std::once_flag flag;
    static void Ensure_Initialised();

    static BB Generate_Rook_Attacks(int square);
    static BB Generate_Rook_Attacks_With_Blocks(int square, BB block);
    static BB Generate_Bishop_Attacks(int square);
    static BB Generate_Bishop_Attacks_With_Blocks(int square, BB block);

    static BB Generate_Pawn_Moves(int square, PieceColour colour);
    static BB Generate_Pawn_Attacks(int square, PieceColour colour);
    static BB Generate_Knight_Attacks(int square);
    static BB Generate_King_Attacks(int square);
    static BB Generate_Between_Squares(int sq1, int sq2);

    static void Initialise_Attacks();

public:

    static BB Get_Pawn_Moves(int square, PieceColour colour, BB occupancy);
    static BB Get_Pawn_Attacks(int square, PieceColour colour, BB enemy, int ep_square);
    static BB Get_Knight_Moves(int square, BB occupancy);
    static BB Get_Bishop_Moves(int square, BB occupancy);
    static BB Get_Rook_Moves(int square, BB occupancy);
    static BB Get_Queen_Moves(int square, BB occupancy);
    static BB Get_King_Moves(int square, BB occupancy);

    static BB Get_Between_Squares(int square1, int square2);
    static BB Get_Attacks_From_Square(int square, PieceColour colour, const Board& board);
    static BB Generate_All_Attacks(PieceColour attacking_colour, const Board& board);
};