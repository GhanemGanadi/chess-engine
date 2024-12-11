#pragma once
#include "../core/Bitboard_Operations.h"
#include "../Moves/Move.h"

namespace Board_Analyser {
    int Is_King_In_Check(U64 kingBit, PieceColour colour, const Board& board);
    bool Is_Checkmate(PieceColour colour, Board& board);
    bool Is_Stalemate(const Board& board, bool isWhiteTurn);

    PieceType Find_Piece_Type(int square, PieceColour colour, const Board& board);

    bool Handle_Castling(const Board& board, const Move& move);
    bool Can_Castle(const Move &move, const Board &board);

    bool Can_Promote(const Move& move);
    void Promote_Pawn(const Move& move, PieceType promotionPiece, Board& board);

    bool Can_En_Passant(const Move& move, const Board& board);
    bool Handle_En_Passant(Move& move, Board& board);

    void Handle_Captures(Move& move, Board& board);
    bool Make_Move(Move& move, bool moveGeneration, Board& board);
    void Move_Piece(const Move& move, Board& board);

    bool Is_Insufficient_Material(const Board& board);
    bool Is_Light_Square_Bishop(U64 bishopBitboard);

    void Update_Half_Clock(const Move& move, Board& board);
    bool Is_Fifty_Move_Draw(Board& board);

    void Set_Move_Flags(Move& move, const Board& board);
    void Debug_Board_State(const Board& board, const std::string& filename);
}

