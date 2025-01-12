#include "Move_Generation.h"

std::array<U64, 64> MoveGeneration::kingMoves;
std::array<std::array<U64, 64>, 2> MoveGeneration::pawnMoves;
std::array<std::array<U64, 64>, 2> MoveGeneration::pawnAttacks;
std::array<U64, 64> MoveGeneration::knightMoves;
Board MoveGeneration::board;


U64 MoveGeneration::Generate_Knight_Moves(const int square) {
    const U64 knight = 1ULL << square;
    return (knight << 6  & ~GH_FILE) | (knight >> 6  & ~AB_FILE) |
           (knight << 10 & ~AB_FILE) | (knight >> 10 & ~GH_FILE) |
           (knight << 15 & ~H_FILE) | (knight >> 15 & ~A_FILE) |
           (knight << 17 & ~A_FILE) | (knight >> 17 & ~H_FILE);
}

U64 MoveGeneration::Generate_King_Moves(const int square) {
    const U64 king = 1ULL << square;
    return (king << 8) | (king >> 8) |
           (king << 1 & ~A_FILE) | (king >> 1 & ~H_FILE) |
           (king << 7 & ~H_FILE) | (king >> 7 & ~A_FILE) |
           (king << 9 & ~A_FILE) | (king >> 9 & ~H_FILE);
}

U64 MoveGeneration::Generate_Pawn_Moves(const int square, const PieceColour colour) {
    const int direction = colour == WHITE ? -8 : 8;
    if((square + direction) > 63 || (square + direction) < 0) return 0ULL;
    U64 pawn = 1ULL << (square + direction);

    const bool onStartRank = colour == WHITE ? (square >= 48 && square <= 55) :
                                       (square >= 8 && square <= 15);

    if (onStartRank) { pawn |= 1ULL << (square + (2 * direction)); }

    return pawn;
}

U64 MoveGeneration::Generate_Pawn_Attacks(const int square, const PieceColour colour) {
    U64 attacks = 0ULL;
    const U64 pawnBB = 1ULL << square;

    if (colour == WHITE) {
        attacks |= ((pawnBB >> 9) & ~H_FILE); // Capture left
        attacks |= ((pawnBB >> 7) & ~A_FILE); // Capture right
    } else {
        attacks |= ((pawnBB << 7) & ~H_FILE); // Capture left
        attacks |= ((pawnBB << 9) & ~A_FILE); // Capture right
    }

    return attacks;
}

U64 MoveGeneration::Generate_Pawn_En_Passant(const int square, const PieceColour colour, const Board& board) {
    U64 moves = 0ULL;
    if(board.moveHistory.empty()) return moves;

    const Move& previousMove = board.moveHistory.back();

    if(previousMove.Get_Piece_Type() != PAWN ||
       (abs(previousMove.Get_To() - previousMove.Get_From())) != 16) {
        return false;
    }

    const int fromRank = square / 8;
    if(colour == WHITE && fromRank != 3) return moves;
    if(colour == BLACK && fromRank != 4) return moves;

    const int moveFile = square % 8;
    const int captureFile = previousMove.Get_To() % 8;
    if(abs(moveFile - captureFile) != 1) return moves;

    const int captureSquare = previousMove.Get_To() + (colour == WHITE ? -8 : 8);
    moves |= 1ULL << captureSquare;

    return moves;
}

[[nodiscard]] U64 MoveGeneration::Get_Pseudo_Legal_Moves(const int square, const PieceColour colour,
                                                        const PieceType pieceType, const Board& board) {

    const U64 friendlyPieces = colour == WHITE ? board.Get_White_Pieces() : board.Get_Black_Pieces();
    const U64 enemyPieces = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();
    const U64 occupancy = friendlyPieces | enemyPieces;

    switch(pieceType) {
        case PAWN: {
            U64 pawnMoves = Get_Pawn_Moves(square, colour);
            const U64 singlePush = (1ULL << (square + (colour == WHITE ? -8 : 8)));
            const U64 doublePush = (1ULL << (square + (colour == WHITE ? -16 : 16)));
            if (singlePush & occupancy) {
                pawnMoves &= ~singlePush;
                pawnMoves &= ~doublePush;
            }

            else if (doublePush & occupancy) {
                pawnMoves &= ~doublePush;
            }

            return (pawnMoves & ~occupancy) |
                (Get_Pawn_Attacks(square, colour) & enemyPieces) |
                Generate_Pawn_En_Passant(square, colour, board);
        }
        case KNIGHT:
            return Get_Knight_Moves(square) & ~friendlyPieces;
        case ROOK:
            return Tables::Get_Rook_Moves(square, occupancy) & ~friendlyPieces;
        case BISHOP:
            return Tables::Get_Bishop_Moves(square, occupancy) & ~friendlyPieces;
        case QUEEN:
            return Tables::Get_Queen_Moves(square, occupancy) & ~friendlyPieces;
        case KING:{
            return (Get_King_Moves(square) & ~friendlyPieces) | Get_King_Castle_Moves(colour, board);}

        default:
            return 0ULL;
    }
}

U64 MoveGeneration::Filter_Legal_Moves(U64 moves, const int square, const PieceColour colour,
                                        const PieceType pieceType, Board& board) {

    U64 legalMoves = 0ULL;
    U64 kingBB = board.Get_Piece_Bitboard(KING, colour);
    const U64 enemyBB = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();

    while(moves) {
        const int moveSquare = Get_LS1B_Index(moves);
        moves &= moves - 1;

        Move tempMove(static_cast<Squares>(square), static_cast<Squares>(moveSquare),
                     pieceType, colour);

        if((1ULL << moveSquare) & enemyBB) {
            Board_Analyser::Handle_Captures(tempMove, board);
        }

        if (pieceType == PAWN){ Board_Analyser::Handle_En_Passant(tempMove, board); }

        // Board_Analyser::Make_Move(tempMove, true, board);
        Board_Analyser::Move_Piece(tempMove, board);
        Board_Analyser::Set_Move_Flags(tempMove, board);
        board.moveHistory.push_back(tempMove);

        if(pieceType == KING) { kingBB = 1ULL << moveSquare; }

        if(Board_Analyser::Is_King_In_Check(kingBB, colour, board) == -1) {
            legalMoves |= (1ULL << moveSquare);
        }
        board.Undo_Move(true);
    }

    return legalMoves;
}


U64 MoveGeneration::Get_Legal_Moves(const int square, const PieceColour colour, const PieceType pieceType, Board &board) {

    const U64 pseudoLegalMoves = Get_Pseudo_Legal_Moves(square, colour, pieceType, board);
    return Filter_Legal_Moves(pseudoLegalMoves, square, colour, pieceType, board);
}

Move MoveGeneration::Detect_Capture(Move& move, const Board& board) {
    const PieceColour colour = move.Get_Colour();
    const U64 pieceLocation = board.Get_Piece_Bitboard(move.Get_Piece_Type(), colour);
    const PieceColour enemyColour = colour == WHITE ? BLACK : WHITE;
    const U64 enemyPieces = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();

    if(pieceLocation & enemyPieces) {
        for(const PieceType pieceType : {PAWN, KNIGHT, ROOK, BISHOP, QUEEN, KING}) {
            if(pieceLocation & board.Get_Piece_Bitboard(pieceType, enemyColour)) {
                move.Set_Capture(true);
                move.Set_Piece_Type(pieceType);
            }
        }
    }
    return move;
}

 U64 MoveGeneration::Get_King_Moves(const int square) {return kingMoves[square];}

[[nodiscard]] U64 MoveGeneration::Get_King_Castle_Moves(const PieceColour colour, const Board& board) {
    U64 moves = 0ULL;
    const U64 occupancy = board.Get_All_Pieces();
    if(colour == WHITE) {
        if(board.Has_White_King_Side_Castling_Rights()) {
            constexpr U64 kingSideMask = 1ULL << f1 | 1ULL << g1;
            if (!(occupancy & kingSideMask)) {
                moves |= 1ULL << g1;
            }
        }
        if(board.Has_White_Queen_Side_Castling_Rights()) {
            constexpr U64 queenSideMask = 1ULL << c1 | 1ULL << d1;
            if (!(occupancy & queenSideMask)) {
                moves |= 1ULL << c1;
            }
        }
    }
    else {
        if(board.Has_Black_King_Side_Castling_Rights()) {
            constexpr U64 kingSideMask = 1ULL << f8 | 1ULL << g8;
            if (!(occupancy & kingSideMask)) {
                moves |= 1ULL << g8;
            }
        }
        if(board.Has_Black_Queen_Side_Castling_Rights()) {
            constexpr U64 queenSideMask = 1ULL << c8 | 1ULL << d8;
            if (!(occupancy & queenSideMask)) {
                moves |= 1ULL << c8;
            }
        }
    }

    return moves;
}
[[nodiscard]] U64 MoveGeneration::Get_Knight_Moves(const int square) {return knightMoves[square];}
[[nodiscard]] U64 MoveGeneration::Get_Pawn_Moves(const int square, const PieceColour colour){return pawnMoves[colour == WHITE ? 0 : 1][square];}
[[nodiscard]] U64 MoveGeneration::Get_Pawn_Attacks(const int square, const PieceColour colour) {return pawnAttacks[colour == WHITE ? 0 : 1][square];
}

std::vector<Move> MoveGeneration::Generate_All_Moves(const PieceColour colour, Board& board) {
    std::vector<Move> allMoves;

    for(const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
        U64 pieceBB = board.Get_Piece_Bitboard(piece, colour);

        while(pieceBB) {
            const int from = Get_LS1B_Index(pieceBB);
            pieceBB &= (pieceBB - 1);

            U64 legalMoves = Get_Legal_Moves(from, colour, piece, board);

            while(legalMoves) {
                const int to = Get_LS1B_Index(legalMoves);
                legalMoves &= (legalMoves - 1);

                Move tempMove(static_cast<Squares>(from), static_cast<Squares>(to), piece, colour);

                if (piece == PAWN) {
                    if (Board_Analyser::Can_Promote(tempMove)) {

                        for (const PieceType differentPieces : {KNIGHT, BISHOP, ROOK, QUEEN}) {
                            Move promotionMove = tempMove;
                            promotionMove.Set_Promotion_Piece(differentPieces);
                            Board_Analyser::Set_Move_Flags(promotionMove, board);

                            if (Board_Analyser::Make_Move(promotionMove, true, board)) {
                                allMoves.push_back(promotionMove);
                                board.Undo_Move(true);
                            }
                        }
                        continue;
                    }
                }

                if (Board_Analyser::Make_Move(tempMove, true, board)) {
                    board.Undo_Move(true);
                    // Board_Analyser::Set_Move_Flags(tempMove, board);
                    allMoves.push_back(tempMove);
                }
            }
        }
    }

    return allMoves;
}
void MoveGeneration::Display_All_Moves(const std::vector<Move>& moves) {
    if(moves.empty()) return;

    PieceType currentType = moves[0].Get_Piece_Type();
    PieceColour currentColor = moves[0].Get_Colour();

    // Print initial piece type header
    std::cout << (currentColor == WHITE ? "White " : "Black ")
              << Get_Piece_Name(currentType) << ":\n";

    for(const Move& move : moves) {
        // If piece type changes, print new header
        if(currentType != move.Get_Piece_Type() || currentColor != move.Get_Colour()) {
            currentType = move.Get_Piece_Type();
            currentColor = move.Get_Colour();
            std::cout << "\n" << (currentColor == WHITE ? "White " : "Black ")
                     << Get_Piece_Name(currentType) << ":\n";
        }

        // Print basic move
        std::cout << Square_To_String(move.Get_From())
                 << Square_To_String(move.Get_To());

        // Print special move types
        if(move.Is_Capture()) std::cout << " (CAPTURE)";
        if(move.Is_Castling()) std::cout << " (CASTLE)";
        if(move.Is_En_Passant()) std::cout << " (EN PASSANT)";
        if(move.Get_Promotion_Piece() != NO_PIECE) {
            std::cout << " (PROMOTE to " << Get_Piece_Name(move.Get_Promotion_Piece()) << ")";
        }

        std::cout << " | ";
    }
    std::cout << std::endl;
}

U64 MoveGeneration::Get_Piece_Attacks(const PieceType piece, const int square, const PieceColour colour, const Board& board) {
    const U64 occupancy = board.Get_All_Pieces();
    const U64 friendlyPieces = colour == WHITE ? board.Get_White_Pieces() : board.Get_Black_Pieces();
    const U64 enemyPieces = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();
    switch(piece) {
        case PAWN: {
            U64 pawnMoves = Get_Pawn_Moves(square, colour);
            const U64 singlePush = (1ULL << (square + (colour == WHITE ? -8 : 8)));
            const U64 doublePush = (1ULL << (square + (colour == WHITE ? -16 : 16)));
            if (singlePush & occupancy) {
                pawnMoves &= ~singlePush;
                pawnMoves &= ~doublePush;
            }

            else if (doublePush & occupancy) {
                pawnMoves &= ~doublePush;
            }

            return (pawnMoves & ~occupancy) |
                (Get_Pawn_Attacks(square, colour) & enemyPieces) |
                Generate_Pawn_En_Passant(square, colour, board);
        }
        case KNIGHT:
            return Get_Knight_Moves(square);
        case BISHOP:
            return Tables::Get_Bishop_Moves(square, occupancy);
        case ROOK:
            return Tables::Get_Rook_Moves(square, occupancy);
        case QUEEN:
            return Tables::Get_Queen_Moves(square, occupancy);
        case KING:
            return Get_King_Moves(square) & ~friendlyPieces;
        default:
            return 0ULL;
    }
}

MoveGeneration::MoveGeneration() {
    Initialise_Lookup_Tables();
    Tables::Initialise_Attacks();
}

void MoveGeneration::Initialise_Lookup_Tables() {
    for (int square = 0; square < 64; square++) {
        knightMoves[square] = Generate_Knight_Moves(square);
        kingMoves[square] = Generate_King_Moves(square);

        pawnMoves[0][square] = Generate_Pawn_Moves(square, WHITE);
        pawnMoves[1][square] = Generate_Pawn_Moves(square, BLACK);
        pawnAttacks[0][square] = Generate_Pawn_Attacks(square, WHITE);
        pawnAttacks[1][square] = Generate_Pawn_Attacks(square, BLACK);
    }
}