#include "Zobrist.h"

int Zobrist::Get_Piece_Index(const PieceColour colour, const PieceType piece) const {
    return (colour * 6) + piece;
}

int Zobrist::Get_Castling_Index(const Board& board) const {
    int index = 0;
    if (board.Has_White_King_Side_Castling_Rights())   index |= 1;
    if (board.Has_White_Queen_Side_Castling_Rights())  index |= 2;
    if (board.Has_Black_King_Side_Castling_Rights())   index |= 4;
    if (board.Has_Black_Queen_Side_Castling_Rights())  index |= 8;
    return index;
}

void Zobrist::Initialise() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<U64> dis;

    for (int i = 0; i < PIECE_TYPES; i++) {
        for (int j = 0; j < SQUARES; j++) {
            Piece_Square[i][j] = dis(gen);
        }
    }
    
    for (int i = 0; i < 16; i++) {
        Castling[i] = dis(gen);
    }
    
    for (int i = 0; i < 8; i++) {
        En_Passant_File[i] = dis(gen);
    }
    
    Side_To_Move = dis(gen);
}

U64 Zobrist::Calculate_Hash(const Board& board) const {
    U64 hash = 0;

    for (const PieceColour colour : {WHITE, BLACK}) {
        for (const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            U64 pieceBB = board.Get_Piece_Bitboard(piece, colour);
            while (pieceBB) {
                int position = Get_LS1B_Index(pieceBB);
                pieceBB &= pieceBB - 1;
                int pieceIndex = Get_Piece_Index(colour, piece);
                hash ^= Piece_Square[pieceIndex][position];
            }
        }
    }
    
    hash ^= Castling[Get_Castling_Index(board)];
    hash ^= Side_To_Move;
    return hash;
}