// Move_Generation.h
#pragma once
#include "Tables.h"
#include "../core/Bitboard_Operations.h"
#include "../Board/Board.h"
#include "../Board/Board_Analyser.h"

class MoveGeneration {
    Board board;
    std::array<U64, 64> knightMoves;
    static std::array<U64, 64> kingMoves;
    std::array<std::array<U64, 64>, 2> pawnMoves;
    std::array<std::array<U64, 64>, 2> pawnAttacks;

    // Private helper functions
    static U64 Generate_Knight_Moves(int square);
    static U64 Generate_King_Moves(int square);
    static U64 Generate_Pawn_Moves(int square, PieceColour colour);
    static U64 Generate_Pawn_Attacks(int square, PieceColour colour);
    static U64 Generate_Pawn_En_Passant(int square, PieceColour colour, const Board& board);

    [[nodiscard]] U64 Get_Pseudo_Legal_Moves(int square, PieceColour colour, PieceType pieceType,
                                            const Board& board) const;

    static U64 Filter_Legal_Moves(U64 moves, int square, PieceColour colour,
                                 PieceType pieceType, Board& board);

    static Move Detect_Capture(Move& move, const Board& board);
    void Initialise_Lookup_Tables();

public:
    MoveGeneration();

    // Public interface
    [[nodiscard]] static U64 Get_King_Moves(int square);
    [[nodiscard]] static U64 Get_King_Castle_Moves(PieceColour colour, const Board& board) ;
    [[nodiscard]] U64 Get_Knight_Moves(int square) const;
    [[nodiscard]] U64 Get_Pawn_Moves(int square, PieceColour colour) const;
    [[nodiscard]] U64 Get_Pawn_Attacks(int square, PieceColour colour) const;

    U64 Get_Legal_Moves(int square, PieceColour colour, PieceType pieceType, Board& board) const;
    std::vector<Move> Generate_All_Moves(PieceColour colour, Board& board) const;
    static void Display_All_Moves(const std::vector<Move>& moves);

    U64 Get_Piece_Attacks(PieceType piece, int square, PieceColour colour) const;
};
