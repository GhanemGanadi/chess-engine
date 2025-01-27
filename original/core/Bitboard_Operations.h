#pragma once
#include <iostream>

// inline unsigned int Random_State_Number = 1804289383;
#define U64 uint64_t
#define A_FILE 72340172838076673ULL
#define H_FILE 9259542123273814144ULL
#define AB_FILE 217020518514230019ULL
#define GH_FILE 13889313184910721216ULL

static constexpr U64 FILE_A = 0x0101010101010101ULL;
static constexpr U64 FILE_B = FILE_A << 1;
static constexpr U64 FILE_C = FILE_A << 2;
static constexpr U64 FILE_D = FILE_A << 3;
static constexpr U64 FILE_E = FILE_A << 4;
static constexpr U64 FILE_F = FILE_A << 5;
static constexpr U64 FILE_G = FILE_A << 6;
static constexpr U64 FILE_H = FILE_A << 7;

static constexpr std::array FILES = {
    FILE_A, FILE_B, FILE_C, FILE_D,
    FILE_E, FILE_F, FILE_G, FILE_H
};

constexpr U64 RANK_1 = 18374686479671623680ULL;
constexpr U64 RANK_2 = 71776119061217280ULL;
constexpr U64 RANK_3 = 280375465082880ULL;
constexpr U64 RANK_4 = 1095216660480ULL;
constexpr U64 RANK_5 = 4278190080ULL;
constexpr U64 RANK_6 = 16711680ULL;
constexpr U64 RANK_7 = 65280ULL;
constexpr U64 RANK_8 = 255ULL;

constexpr std::array RANKS = {
    RANK_1, RANK_2, RANK_3, RANK_4,
    RANK_5, RANK_6, RANK_7, RANK_8
};

enum PieceType {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE};
enum PieceColour {WHITE, BLACK, NO_COLOUR};

enum Squares {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};



inline bool Get_Bit(const U64 bitboard, const int square) {
    return bitboard & (1ULL << square);
}

inline U64 Set_Bit(U64 bitboard, const int square) {
    return bitboard |= (1ULL << square);
}

inline U64 Remove_Bit(const U64 bitboard, const int square) {
    return bitboard & (~(1ULL << square));
}

inline int Get_Square(const U64 occupancy) {
        for(int square = 0; square < 64; ++square) {
            if(occupancy & (1ULL << square)){return square;}
        }
        return -1;
    }
inline int Count_Bits(U64 bitboard) {
    int count = 0;
    for(int bit = 0; bit < 64; bit++) {
        if(Get_Bit(bitboard, bit)) {
            count++;
            bitboard &= bitboard - 1;
        }
    }
    return count;
}

inline int Get_LS1B_Index(const U64 bitboard) {
    return bitboard ? Count_Bits((bitboard & -bitboard) - 1 ) : -1;
}

inline U64 Get_MS1B(U64 bitboard) {
    bitboard |= bitboard >> 32;
    bitboard |= bitboard >> 16;
    bitboard |= bitboard >> 8;
    bitboard |= bitboard >> 4;
    bitboard |= bitboard >> 2;
    bitboard |= bitboard >> 1;

    return bitboard ^= bitboard >> 1;
}

inline void Print_Bitboard(const U64 bitboard) {
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
    std:: cout << "   a b c d e f g h" << std::endl;
    std::cout << std::endl;
}

inline U64 Set_Occupancy(const int index, const int bitsInMask, U64 attackMask){
    U64 occupancy = 0ULL;
    for(int count = 0; count < bitsInMask; count++){
        const int square = Get_LS1B_Index(attackMask);

        attackMask = Remove_Bit(attackMask, square);

        if(index & (1 << count)) {
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}

// inline unsigned int Random_Number() {
//     unsigned int number = Random_State_Number;
//     number ^= number << 13;
//     number ^= number >> 17;
//     number ^= number << 5;
//     Random_State_Number = number;
//     return number;
// }

inline PieceType Choose_Piece_Promote() {
    std::cout << "Choose piece to promote to:\n";
    std::cout << "1. Queen\n";
    std::cout << "2. Rook\n";
    std::cout << "3. Bishop\n";
    std::cout << "4. Knight\n";

    int choice;
    while (true) {
        std::cin >> choice;
        switch (choice) {
            case 1: return QUEEN;
            case 2: return ROOK;
            case 3: return BISHOP;
            case 4: return KNIGHT;
            default:
                std::cout << "Invalid choice. Please select 1-4:\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

inline U64 Parse_Fen(const std::string& fen, const bool isWhite, const PieceType piece) {
    int square = 0;
    U64 bitboard = 0ULL;

    // Define which character we're looking for based on piece type and color
    char targetPiece;
    switch(piece) {
        case PAWN: targetPiece = isWhite ? 'P' : 'p'; break;
        case ROOK: targetPiece = isWhite ? 'R' : 'r'; break;
        case KNIGHT: targetPiece = isWhite ? 'N' : 'n'; break;
        case BISHOP: targetPiece = isWhite ? 'B' : 'b'; break;
        case QUEEN: targetPiece = isWhite ? 'Q' : 'q'; break;
        case KING: targetPiece = isWhite ? 'K' : 'k'; break;
        default: return 0ULL;  // Invalid piece type
    }

    for (char c : fen) {
        if (c == '/') {
            continue;  // Skip to next rank
        }
        else if (isdigit(c)) {
            square += (c - '0');  // Skip empty squares
        }
        else if (c == targetPiece) {
            bitboard |= 1ULL << square;
            square++;
        }
        else if (isalpha(c)) {
            square++;  // Skip other pieces
        }

        if (square >= 64) break;
    }

    return bitboard;
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

inline Squares String_To_Square(const std::string& str) {
    if (str.length() != 2) return a8;

    const std::string files = "abcdefgh";
    const std::string ranks = "87654321";

    int file = files.find(std::tolower(str[0]));
    if (file == std::string::npos) return a8;

    int rank = ranks.find(str[1]);
    if (rank == std::string::npos) return a8;

    return static_cast<Squares>(rank * 8 + file);
}

inline std::string Get_Piece_Name(PieceType piece) {
    switch(piece) {
        case PAWN: return "Pawn";
        case ROOK: return "Rook";
        case KNIGHT: return "Knight";
        case BISHOP: return "Bishop";
        case QUEEN: return "Queen";
        case KING: return "King";
        case NO_PIECE: return "No Piece";
        default: return "Unknown";
    }
}
inline U64 Get_Path_Between(const int from, const int to) {
    U64 path = 0ULL;
    int rankDiff = (to / 8) - (from / 8);
    int fileDiff = (to % 8) - (from % 8);

    // Diagonal path
    if (abs(rankDiff) == abs(fileDiff)) {
        int rankStep = (rankDiff > 0) ? 1 : -1;
        int fileStep = (fileDiff > 0) ? 1 : -1;
        int current = from;
        while (current != to) {
            current += (rankStep * 8) + fileStep;
            if (current != to)
                path |= 1ULL << current;
        }
    }
    // Straight path
    else if (rankDiff == 0 || fileDiff == 0) {
        int step = rankDiff ? 8 : 1;
        if (rankDiff < 0 || fileDiff < 0) step = -step;
        int current = from;
        while (current != to) {
            current += step;
            if (current != to) // Don't include the destination square
                path |= 1ULL << current;
        }
    }
    return path | (1ULL << to); // Include the checking piece's square
}