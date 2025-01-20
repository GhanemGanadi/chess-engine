#include "Zobrist.h"

int Zobrist::Get_Piece_Index(const PieceColour colour, const PieceType piece) {
    return (colour * 6) + piece;
}


void Zobrist::Initialise() {
    std::mt19937_64 gen(12345);
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
U64 Zobrist::Calculate_Hash_Initial_Position() const {
    U64 hash = 0;

    // White pieces - back rank
    hash ^= Piece_Square[Get_Piece_Index(WHITE, ROOK)][a1];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, KNIGHT)][b1];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, BISHOP)][c1];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, QUEEN)][d1];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, KING)][e1];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, BISHOP)][f1];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, KNIGHT)][g1];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, ROOK)][h1];

    hash ^= Piece_Square[Get_Piece_Index(WHITE, PAWN)][a2];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, PAWN)][b2];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, PAWN)][c2];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, PAWN)][d2];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, PAWN)][e2];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, PAWN)][f2];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, PAWN)][g2];
    hash ^= Piece_Square[Get_Piece_Index(WHITE, PAWN)][h2];

    hash ^= Piece_Square[Get_Piece_Index(BLACK, ROOK)][a8];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, KNIGHT)][b8];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, BISHOP)][c8];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, QUEEN)][d8];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, KING)][e8];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, BISHOP)][f8];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, KNIGHT)][g8];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, ROOK)][h8];

    hash ^= Piece_Square[Get_Piece_Index(BLACK, PAWN)][a7];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, PAWN)][b7];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, PAWN)][c7];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, PAWN)][d7];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, PAWN)][e7];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, PAWN)][f7];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, PAWN)][g7];
    hash ^= Piece_Square[Get_Piece_Index(BLACK, PAWN)][h7];

    hash ^= Castling[15];
    return hash;
}


U64 Zobrist::Zobrist_Make_Move(const Move& move, U64 hash) const {
    int pieceIndex = Get_Piece_Index(move.Get_Colour(), move.Get_Piece_Type());
    hash ^= Piece_Square[pieceIndex][move.Get_From()];

    if (move.Get_Promotion_Piece() != NO_PIECE) {
        pieceIndex = Get_Piece_Index(move.Get_Colour(), move.Get_Promotion_Piece());
    }

    hash ^= Piece_Square[pieceIndex][move.Get_To()];

    if (move.Is_Capture()) {
        const int captureSquare = move.Get_Captured_Destination();
        pieceIndex = Get_Piece_Index(move.Get_Colour() == WHITE ? BLACK : WHITE, move.Get_Captured_Piece());
        hash ^= Piece_Square[pieceIndex][captureSquare];
    }

    if (move.Is_Castling()) {
        hash ^= Castling[move.Get_Colour() == WHITE ? 0x3 : 0xC];


        const int rookIndex = Get_Piece_Index(move.Get_Colour(), ROOK);
        const int rookFromSquare = move.Get_Castle_Side() == Castle_Side::King_Side ?
                                                            (move.Get_Colour() == WHITE ? h1 : h8) :
                                                            (move.Get_Colour() == WHITE ? a1 : a8);

        const int rookToSquare = move.Get_Castle_Side() == Castle_Side::King_Side ?
                                                            (move.Get_Colour() == WHITE ? f1 : f8) :
                                                            (move.Get_Colour() == WHITE ? d1 : d8);

        hash ^= Piece_Square[rookIndex][rookFromSquare];
        hash ^= Piece_Square[rookIndex][rookToSquare];
    }
    else if (move.Get_Piece_Type() == KING) {
        hash ^= Castling[move.Get_Colour() == WHITE ? 0x3 : 0xC];
    }
    else if (move.Get_Piece_Type() == ROOK) {
        if (move.Get_From() == a1) hash ^= Castling[0x2];
        else if (move.Get_From() == h1) hash ^= Castling[0x1];
        else if (move.Get_From() == a8) hash ^= Castling[0x8];
        else if (move.Get_From() == h8) hash ^= Castling[0x4];
    }

    hash ^= Side_To_Move;

    return hash;
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
    if (board.currentTurn == BLACK){ hash ^= Side_To_Move; }
    return hash;
}

int Zobrist::Get_Castling_Index(const Board& board) const {
    int index = 0;
    if (board.Has_White_King_Side_Castling_Rights())   index |= 1;
    if (board.Has_White_Queen_Side_Castling_Rights())  index |= 2;
    if (board.Has_Black_King_Side_Castling_Rights())   index |= 4;
    if (board.Has_Black_Queen_Side_Castling_Rights())  index |= 8;
    return index;
}