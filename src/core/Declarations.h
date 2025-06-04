#pragma once
#include <cstdint>
#include <iostream>

using U32 = std::uint32_t;
using BB = std::uint64_t;

static constexpr BB AB_FILE = 217020518514230019ULL;
static constexpr BB GH_FILE =  13889313184910721216ULL;

static constexpr BB A_FILE = 0x0101010101010101ULL;
static constexpr BB B_FILE = A_FILE << 1;
static constexpr BB C_FILE = A_FILE << 2;
static constexpr BB D_FILE = A_FILE << 3;
static constexpr BB E_FILE = A_FILE << 4;
static constexpr BB F_FILE = A_FILE << 5;
static constexpr BB G_FILE = A_FILE << 6;
static constexpr BB H_FILE = A_FILE << 7;

static constexpr std::array FILES = {
                                    A_FILE, B_FILE, C_FILE, D_FILE,
                                    E_FILE, F_FILE, G_FILE, H_FILE
                                    };

static constexpr BB RANK_1 = 18374686479671623680ULL;
static constexpr BB RANK_2 = 71776119061217280ULL;
static constexpr BB RANK_3 = 280375465082880ULL;
static constexpr BB RANK_4 = 1095216660480ULL;
static constexpr BB RANK_5 = 4278190080ULL;
static constexpr BB RANK_6 = 16711680ULL;
static constexpr BB RANK_7 = 65280ULL;
static constexpr BB RANK_8 = 255ULL;

static constexpr std::array RANKS = {
                                    RANK_1, RANK_2, RANK_3, RANK_4,
                                    RANK_5, RANK_6, RANK_7, RANK_8
                                    };

enum PieceType {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE};
enum PieceColour {WHITE, BLACK};
enum CastleSide {NO_CASTLE, KING_SIDE, QUEEN_SIDE};
enum Squares {
    a8=0, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

inline bool Get_Bit(const BB bitboard, const int square) {
    return bitboard & (1ULL << square);
}

inline int Count_Bits(const BB bitboard) {
    return __builtin_popcountll(bitboard);
}

inline int Get_LSB(const BB bitboard) {
    return bitboard ? __builtin_ctzll(bitboard) : -1;
}

inline BB Set_Occupancy(const int index, const int bits_in_mask, BB attack_mask){
    BB occupancy = 0ULL;
    for(int count = 0; count < bits_in_mask; count++){
        const int square = Get_LSB(attack_mask);

        attack_mask &= attack_mask - 1;

        if(index & (1 << count)) {
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}

inline void Print_Bitboard(const BB bitboard) {
    int count = 8;
    for(int file = 0; file < 8; file++) {
        std::cout << count << "  ";

        for(int rank = 0; rank < 8; rank++) {
            int position = file * 8 + rank;

            if(Get_Bit(bitboard, position)) {std::cout << 1 << " ";}
            else {std::cout  << ". ";}

        }
        std::cout << std::endl;
        count -= 1;
    }
    std::cout << "   a b c d e f g h" << std::endl;
    std::cout << std::endl;
 }

[[nodiscard]] inline bool Same_Diagonal(const int square1, const int square2) {
    const int rank1 = square1 / 8;
    const int file1 = square1 % 8;
    const int rank2 = square2 / 8;
    const int file2 = square2 % 8;

    return (rank1 - file1 == rank2 - file2) || (rank1 + file1 == rank2 + file2);
}

inline std::string Square_To_String(int square) {
    const std::string files = "abcdefgh";
    const std::string ranks = "87654321";

    int file = square % 8;
    int rank = square / 8;

    std::string result;
    result += files[file];
    result += ranks[rank];

    return result;
}

inline std::string Get_Piece_Name(int piece) {
    switch(piece) {
        case 0: return "White Pawn";
        case 1: return "White Rook";
        case 2: return "White Knight";
        case 3: return "White Bishop";
        case 4: return "White Queen";
        case 5: return "White King";
        case 6: return "Black Pawn";
        case 7: return "Black Rook";
        case 8: return "Black Knight";
        case 9: return "Black Bishop";
        case 10: return "Black Queen";
        case 11: return "Black King";
        case 12: return "No Piece";
        default: return "Unknown";
    }
}

inline char Get_Piece_Symbol(const int piece) {
    switch (piece) {
        case PAWN:
            return 'p';
        case KNIGHT:
            return 'n';
        case BISHOP:
            return 'b';
        case ROOK:
            return 'r';
        case QUEEN:
            return 'q';
        case KING:
            return 'k';
        default: return 'x';
    }
}


