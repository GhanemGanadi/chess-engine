#pragma once

#include "Declarations.h"

class Move {
    union {
        struct {
            U32 from : 6;
            U32 to : 6;
            U32 colour : 1;
            U32 piece : 3;
            U32 captured_piece : 3;
            U32 capture_destination : 6;
            U32 promotion_piece : 3;
            U32 castle : 2;
            U32 is_en_passant : 1;
        } bits;
        U32 raw;
    };

public:

    constexpr Move() : raw(0) {}

    constexpr Move(const int from, const int to, const PieceType piece, const PieceColour colour) {
        raw = 0;
        bits.from = from;
        bits.to = to;
        bits.piece = piece;
        bits.colour = colour;
        bits.captured_piece = NO_PIECE;
        bits.castle = NO_CASTLE;

    }

    [[nodiscard]] constexpr int Get_From() const { return bits.from; }
    [[nodiscard]] constexpr int Get_To() const { return bits.to; }
    [[nodiscard]] constexpr int Get_Colour() const { return bits.colour; }
    [[nodiscard]] constexpr int Get_Piece() const { return bits.piece; }

    [[nodiscard]] constexpr int Get_Captured_Piece() const { return bits.captured_piece; }
    [[nodiscard]] constexpr int Get_Capture_Position() const { return bits.capture_destination; }
    [[nodiscard]] constexpr int Get_Promotion_Piece() const { return bits.promotion_piece; }
    [[nodiscard]] constexpr int Get_Castle_Side() const { return bits.castle; }
    [[nodiscard]] constexpr int Is_En_Passant() const { return bits.is_en_passant; }

    [[nodiscard]] constexpr U32 GetRaw() const { return raw; }

    constexpr void Set_From(const int from) { bits.from = from; }
    constexpr void Set_To(const int to) { bits.to = to; }
    constexpr void Set_Colour(const PieceColour colour) { bits.colour = colour; }
    constexpr void Set_Piece(const PieceType type) { bits.piece = type; }

    constexpr void Set_Captured_Piece(const PieceType type) { bits.promotion_piece = type; }
    constexpr void Set_Capture_Position(const int square) { bits.capture_destination = square; }
    constexpr void Set_Promotion_Piece(const PieceType type) { bits.promotion_piece = type; }
    constexpr void Set_Castle_Side(const CastleSide side) { bits.castle = side; }
    constexpr void Set_En_Passant(const bool en_passant) { bits.is_en_passant = en_passant; }


    [[nodiscard]] constexpr bool Is_Castling() const {return bits.castle != NO_CASTLE; }

    bool operator==(const Move& other) const { return raw == other.raw; }
    bool operator!=(const Move& other) const { return raw != other.raw; }









};