#pragma once
#include <vector>
#include "../core/Bitboard_Operations.h"

class Tables {
public:
    static U64 Bishop_Moves[64][512];
    static U64 Rook_Moves[64][4096];
    static U64 Bishop_Masks[64];
    static U64 Rook_Masks[64];

    static int Relevant_Rook_Squares[64];
    static int Bishop_Relevant_Squares[64];
    static std::vector<U64> Rook_Magic_Numbers;
    static std::vector<U64>  Bishop_Magic_Numbers;

    static void Initialise_Attacks();
    static U64 Generate_Rook_Attacks(int square);
    static U64 Generate_Rook_Attacks_With_Blocks(int square, U64 block);
    static U64 Generate_Bishop_Attacks(int square);
    static U64 Generate_Bishop_Attacks_With_Blocks(int square, U64 block);
    static U64 Get_Bishop_Moves(int square, U64 occupancy);
    static U64 Get_Rook_Moves(int square, U64 occupancy);
    static U64 Get_Queen_Moves(int square, U64 occupancy);
};