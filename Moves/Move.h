#pragma once
#include "../core/Bitboard_Operations.h"

class Board;

struct Move {
private:
    Squares fromSquare;
    Squares toSquare;
    PieceType pieceType;
    PieceColour pieceColour;
    PieceType capturedPiece;
    int pieceCaptureSquare;
    PieceType promotionPiece;
    bool kingSideCastle;
    bool isCapture;
    bool isCastling;
    bool isEnPassant;

public:
    Move(const Squares from, const Squares to, const PieceType piece, const PieceColour colour)
        : fromSquare(from)
        , toSquare(to)
        , pieceType(piece)
        , pieceColour(colour)
        , promotionPiece(NO_PIECE)
        , capturedPiece(NO_PIECE)
        , pieceCaptureSquare(-1)
        , isCapture(false)
        , isCastling(false)
        , isEnPassant(false)
        , kingSideCastle(false)
    {}

    [[nodiscard]] Squares Get_From() const { return fromSquare; }
    [[nodiscard]] Squares Get_To() const { return toSquare; }
    [[nodiscard]] int Get_Captured_Destination() const { return pieceCaptureSquare; }
    [[nodiscard]] PieceType Get_Piece_Type() const { return pieceType; }
    [[nodiscard]] PieceColour Get_Colour() const { return pieceColour; }
    [[nodiscard]] PieceType Get_Promotion_Piece() const { return promotionPiece; }
    [[nodiscard]] PieceType Get_Captured_Piece() const { return capturedPiece; }
    [[nodiscard]] bool Get_Castle_Side() const { return kingSideCastle; }
    [[nodiscard]] bool Is_Capture() const { return isCapture; }
    [[nodiscard]] bool Is_Castling() const { return isCastling; }
    [[nodiscard]] bool Is_En_Passant() const { return isEnPassant; }

    // Setters
    void Set_Promotion_Piece(const PieceType piece) { promotionPiece = piece; }
    void Set_Capture(const bool capture) { isCapture = capture; }
    void Set_Capture_Destination(const Squares location){ pieceCaptureSquare = location; }
    void Set_Castling(const bool castling) { isCastling = castling; }
    void Set_En_Passant(const bool enPassant) { isEnPassant = enPassant; }
    void Set_From(const Squares from) { fromSquare = from; }
    void Set_To(const Squares to) { toSquare = to; }
    void Set_Piece_Type(const PieceType piece) { pieceType = piece; }
    void Set_Captured_Piece(const PieceType piece) { capturedPiece = piece; }
    void Set_King_Side_Castle(const bool castle){ kingSideCastle = castle; }

};
namespace Move_Parsing {
    [[nodiscard]] Move Parse_Move(const std::string& input, PieceColour colour, const Board& board);
    bool Check_Move(const std::string& fromSquare, const std::string& toSquare);
}

