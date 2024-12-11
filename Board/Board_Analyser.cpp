#include "../Board/Board_Analyser.h"

#include <filesystem>
#include <fstream>
#include <cstring>
#include "../Moves/Move_Generation.h"

namespace Board_Analyser {
    auto Moves_Generation = MoveGeneration();

    void Debug_Board_State(const Board& board, const std::string& filename) {
        std::string fullPath = std::filesystem::current_path().string() + "/" + filename;
        std::ofstream file(fullPath, std::ios::app);

        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << fullPath << std::endl;
            std::cerr << "Error: " << std::strerror(errno) << std::endl;
            return;
        }

        // Write a test line to verify stream is working
        file << "Starting board debug...\n";
        file.flush();

        file << "\n=== DETAILED BOARD STATE ===\n\n";
        file << board.Board_To_Fen() << std::endl;

    // Print visual board representation
    file << "Current Board Position:\n";
    int count = 8;
    for(int file_idx = 0; file_idx < 8; file_idx++) {
        file << count << "  ";
        for(int rank = 0; rank < 8; rank++) {
            int position = file_idx * 8 + rank;
            char pieceChar = '.';

            if(Get_Bit(board.Get_White_Pawn(), position)) pieceChar = 'P';
            else if(Get_Bit(board.Get_White_Knight(), position)) pieceChar = 'N';
            else if(Get_Bit(board.Get_White_Bishop(), position)) pieceChar = 'B';
            else if(Get_Bit(board.Get_White_Rook(), position)) pieceChar = 'R';
            else if(Get_Bit(board.Get_White_Queen(), position)) pieceChar = 'Q';
            else if(Get_Bit(board.Get_White_King(), position)) pieceChar = 'K';
            else if(Get_Bit(board.Get_Black_Pawn(), position)) pieceChar = 'p';
            else if(Get_Bit(board.Get_Black_Knight(), position)) pieceChar = 'n';
            else if(Get_Bit(board.Get_Black_Bishop(), position)) pieceChar = 'b';
            else if(Get_Bit(board.Get_Black_Rook(), position)) pieceChar = 'r';
            else if(Get_Bit(board.Get_Black_Queen(), position)) pieceChar = 'q';
            else if(Get_Bit(board.Get_Black_King(), position)) pieceChar = 'k';

            file << pieceChar << " ";
        }
        file << "\n";
        count--;
    }
    file << "   a b c d e f g h\n\n";
    //
    // // Print bitboard counts
    // file << "Piece Counts:\n";
    // file << "White Pieces:\n";
    // file << "  Pawns:   " << Count_Bits(board.Get_White_Pawn()) << "\n";
    // file << "  Knights: " << Count_Bits(board.Get_White_Knight()) << "\n";
    // file << "  Bishops: " << Count_Bits(board.Get_White_Bishop()) << "\n";
    // file << "  Rooks:   " << Count_Bits(board.Get_White_Rook()) << "\n";
    // file << "  Queens:  " << Count_Bits(board.Get_White_Queen()) << "\n";
    // file << "  Kings:   " << Count_Bits(board.Get_White_King()) << "\n";
    // file << "Total White Pieces: " << Count_Bits(board.Get_White_Pieces()) << "\n";
    //
    // file << "\nBlack Pieces:\n";
    // file << "  Pawns:   " << Count_Bits(board.Get_Black_Pawn()) << "\n";
    // file << "  Knights: " << Count_Bits(board.Get_Black_Knight()) << "\n";
    // file << "  Bishops: " << Count_Bits(board.Get_Black_Bishop()) << "\n";
    // file << "  Rooks:   " << Count_Bits(board.Get_Black_Rook()) << "\n";
    // file << "  Queens:  " << Count_Bits(board.Get_Black_Queen()) << "\n";
    // file << "  Kings:   " << Count_Bits(board.Get_Black_King()) << "\n";
    // file << "Total Black Pieces: " << Count_Bits(board.Get_Black_Pieces()) << "\n";
    //
    // file << "\nTotal Pieces on Board: " << Count_Bits(board.Get_All_Pieces()) << "\n";

    // Helper function to print bitboard to file
    auto PrintBitboardToFile = [&file](U64 bitboard) {
        int inner_count = 8;
        for(int file_idx = 0; file_idx < 8; file_idx++) {
            file << inner_count << "  ";
            for(int rank = 0; rank < 8; rank++) {
                if(Get_Bit(bitboard, file_idx * 8 + rank)) {
                    file << "1 ";
                } else {
                    file << ". ";
                }
            }
            file << "\n";
            inner_count--;
        }
        file << "   a b c d e f g h\n\n";
    };

    // // Print individual piece bitboards
    // file << "\nWhite Piece Bitboards:\n";
    // file << "White Pawns:\n";
    // PrintBitboardToFile(board.Get_White_Pawn());
    // file << "White Knights:\n";
    // PrintBitboardToFile(board.Get_White_Knight());
    // file << "White Bishops:\n";
    // PrintBitboardToFile(board.Get_White_Bishop());
    // file << "White Rooks:\n";
    // PrintBitboardToFile(board.Get_White_Rook());
    // file << "White Queens:\n";
    // PrintBitboardToFile(board.Get_White_Queen());
    // file << "White King:\n";
    // PrintBitboardToFile(board.Get_White_King());
    //
    // file << "\nBlack Piece Bitboards:\n";
    // file << "Black Pawns:\n";
    // PrintBitboardToFile(board.Get_Black_Pawn());
    // file << "Black Knights:\n";
    // PrintBitboardToFile(board.Get_Black_Knight());
    // file << "Black Bishops:\n";
    // PrintBitboardToFile(board.Get_Black_Bishop());
    // file << "Black Rooks:\n";
    // PrintBitboardToFile(board.Get_Black_Rook());
    // file << "Black Queens:\n";
    // PrintBitboardToFile(board.Get_Black_Queen());
    // file << "Black King:\n";
    // PrintBitboardToFile(board.Get_Black_King());
    //
    // // Print combined bitboards
    // file << "\nCombined Bitboards:\n";
    // file << "All White Pieces:\n";
    // PrintBitboardToFile(board.Get_White_Pieces());
    // file << "All Black Pieces:\n";
    // PrintBitboardToFile(board.Get_Black_Pieces());
    // file << "All Pieces:\n";
    // PrintBitboardToFile(board.Get_All_Pieces());

    // Print castling rights
    file << "\nCastling Rights:\n";
    file << "White Kingside:  " << (board.Has_White_King_Side_Castling_Rights() ? "Yes" : "No") << "\n";
    file << "White Queenside: " << (board.Has_White_Queen_Side_Castling_Rights() ? "Yes" : "No") << "\n";
    file << "Black Kingside:  " << (board.Has_Black_King_Side_Castling_Rights() ? "Yes" : "No") << "\n";
    file << "Black Queenside: " << (board.Has_Black_Queen_Side_Castling_Rights() ? "Yes" : "No") << "\n";

    // Print game state
    file << "\nGame State: ";
    switch(board.Get_Game_State()) {
        case Game_State::ACTIVE: file << "Active"; break;
        case Game_State::CHECK: file << "Check"; break;
        case Game_State::CHECKMATE: file << "Checkmate"; break;
        case Game_State::STALEMATE: file << "Stalemate"; break;
        case Game_State::FIFTY_MOVE_DRAW: file << "Fifty Move Draw"; break;
        case Game_State::INSUFFICIENT_MATERIAL: file << "Insufficient Material"; break;
        case Game_State::THREEFOLD_REPETITION: file << "Threefold Repetition"; break;
        case Game_State::DRAW_BY_AGREEMENT: file << "Draw by Agreement"; break;
        case Game_State::DEAD_POSITION: file << "Dead Position"; break;
        case Game_State::RESIGNATION: file << "Resignation"; break;
        case Game_State::TIME_FORFEIT: file << "Time Forfeit"; break;
        default: file << "Unknown"; break;
    }
    file << "\n";

    // Print half move clock
    file << "Half Move Clock: " << board.Get_Half_Clock() << "\n";

    // Print move history size
    file << "Move History Size: " << board.moveHistory.size() << "\n";

    // Print actual move history
    file << "\nMove History:\n";
    for(const auto& move : board.moveHistory) {
        file << "From: " << Square_To_String(move.Get_From())
             << " To: " << Square_To_String(move.Get_To())
             << " Piece: " << Get_Piece_Name(move.Get_Piece_Type());
        if(move.Is_Capture()) file << " (Capture)";
        if(move.Is_Castling()) file << " (Castle)";
        if(move.Is_En_Passant()) file << " (En Passant)";
        if(move.Get_Promotion_Piece() != NO_PIECE) {
            file << " (Promotion to " << Get_Piece_Name(move.Get_Promotion_Piece()) << ")";
        }
        file << "\n";
    }

    // Validation checks
    file << "\nValidation Checks:\n";

    // Check for overlapping pieces
    bool hasOverlap = false;
    U64 occupied = 0ULL;
    const U64 allPieces = board.Get_All_Pieces();
    if(Count_Bits(allPieces) != Count_Bits(board.Get_White_Pieces()) + Count_Bits(board.Get_Black_Pieces())) {
        file << "ERROR: Piece count mismatch between combined bitboards!\n";
        hasOverlap = true;
    }

    // Check for each piece type
    for(const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
        U64 whitePiece = board.Get_Piece_Bitboard(piece, WHITE);
        U64 blackPiece = board.Get_Piece_Bitboard(piece, BLACK);

        if(whitePiece & occupied) {
            file << "ERROR: White " << Get_Piece_Name(piece) << " overlaps with other pieces!\n";
            hasOverlap = true;
        }
        if(blackPiece & occupied) {
            file << "ERROR: Black " << Get_Piece_Name(piece) << " overlaps with other pieces!\n";
            hasOverlap = true;
        }
        occupied |= whitePiece | blackPiece;
    }

    if(!hasOverlap) {
        file << "No piece overlaps detected\n";
    }

    // Check king count
    if(Count_Bits(board.Get_White_King()) != 1) {
        file << "ERROR: Invalid number of white kings: " << Count_Bits(board.Get_White_King()) << "\n";
    }
    if(Count_Bits(board.Get_Black_King()) != 1) {
        file << "ERROR: Invalid number of black kings: " << Count_Bits(board.Get_Black_King()) << "\n";
    }

    // Check for pawns on first/last ranks
    U64 firstRank = 0xFFULL;
    U64 lastRank = 0xFF00000000000000ULL;
    if(board.Get_White_Pawn() & firstRank) {
        file << "ERROR: White pawn(s) found on first rank!\n";
    }
    if(board.Get_White_Pawn() & lastRank) {
        file << "ERROR: White pawn(s) found on last rank!\n";
    }
    if(board.Get_Black_Pawn() & firstRank) {
        file << "ERROR: Black pawn(s) found on first rank!\n";
    }
    if(board.Get_Black_Pawn() & lastRank) {
        file << "ERROR: Black pawn(s) found on last rank!\n";
    }

    file << "\n=== END BOARD STATE ===\n\n";
    file << "----------------------------------------------------------------\n\n";

    file.close();
}

    int Is_Square_Attacked(const int square, const PieceColour attackerColour, const Board& board) {

        // Check pawn attacks on square
        const U64 pawnAttacks = Moves_Generation.Get_Pawn_Attacks(square, attackerColour == WHITE ? BLACK : WHITE);
        if (pawnAttacks & board.Get_Piece_Bitboard(PAWN, attackerColour)) {
            return Get_Square(pawnAttacks & board.Get_Piece_Bitboard(PAWN, attackerColour));
        }

        // Check knight attacks on square
        U64 knightAttacks = Moves_Generation.Get_Knight_Moves(square);
        if (knightAttacks & board.Get_Piece_Bitboard(KNIGHT, attackerColour)) {
            return Get_Square(knightAttacks & board.Get_Piece_Bitboard(KNIGHT, attackerColour));
        }

        // Check king attacks on square
        U64 kingAttacks = Moves_Generation.Get_King_Moves(square);
        if (kingAttacks & board.Get_Piece_Bitboard(KING, attackerColour)){
            return Get_Square(kingAttacks & board.Get_Piece_Bitboard(KING, attackerColour));
        }

        // Check sliding pieces
        U64 occupancy = board.Get_All_Pieces();

        // Check rook/queen attacks
        U64 rookAttacks = Tables::Get_Rook_Moves(square, occupancy);
        if (rookAttacks & (board.Get_Piece_Bitboard(ROOK, attackerColour))) {
            return Get_Square(rookAttacks & board.Get_Piece_Bitboard(ROOK, attackerColour));
        }

        // Check bishop/queen attacks
        U64 bishopAttacks = Tables::Get_Bishop_Moves(square, occupancy);
        if (bishopAttacks & (board.Get_Piece_Bitboard(BISHOP, attackerColour))) {
            return Get_Square((bishopAttacks & board.Get_Piece_Bitboard(BISHOP, attackerColour)));
        }

         U64 queenAttacks = Tables::Get_Queen_Moves(square, occupancy);
         if (queenAttacks & board.Get_Piece_Bitboard(QUEEN, attackerColour)) {
             return Get_Square(queenAttacks & board.Get_Piece_Bitboard(QUEEN, attackerColour));
         }

        return -1;
    }

    bool Set_Piece_Bitboard(Board& board, const PieceType piece, const PieceColour colour, const U64 bitboard) {
        if (piece == NO_PIECE) return false;

        if(colour == WHITE) {
            switch(piece) {
                case PAWN: board.Set_White_Pawn(bitboard); break;
                case ROOK: board.Set_White_Rook(bitboard); break;
                case KNIGHT: board.Set_White_Knight(bitboard); break;
                case BISHOP: board.Set_White_Bishop(bitboard); break;
                case QUEEN: board.Set_White_Queen(bitboard); break;
                case KING: board.Set_White_King(bitboard); break;
                default: return false;
            }
        } else {
            switch(piece) {
                case PAWN: board.Set_Black_Pawn(bitboard); break;
                case ROOK: board.Set_Black_Rook(bitboard); break;
                case KNIGHT: board.Set_Black_Knight(bitboard); break;
                case BISHOP: board.Set_Black_Bishop(bitboard); break;
                case QUEEN: board.Set_Black_Queen(bitboard); break;
                case KING: board.Set_Black_King(bitboard); break;
                default: return false;
            }
        }

        return true;
    }

    int Is_King_In_Check(const U64 kingBit, const PieceColour colour, const Board& board) {
        const int kingSquare = Get_LS1B_Index(kingBit);
        const PieceColour enemyColour = colour == WHITE ? BLACK : WHITE;
        return Is_Square_Attacked(kingSquare, enemyColour, board);
     }

    PieceType Find_Piece_Type(const int square, const PieceColour colour, const Board& board) {
        const U64 bitboard = 1ULL << square;
        //blackPawn, blackKnight, blackBishop, blackRook, blackQueen, blackKing

        for(const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            if(board.Get_Piece_Bitboard(piece, colour) & bitboard) {
                return piece;
            }
        }

        return NO_PIECE;
    }

    bool Is_Checkmate(const PieceColour colour, Board& board) {

         const U64 kingBB = board.Get_Piece_Bitboard(KING, colour);
         const int kingSquare = Get_Square(kingBB);
         const int attackerSquare = Is_King_In_Check(kingBB, colour, board);
         if (attackerSquare == -1){return false;}

         const U64 legalMoves = Moves_Generation.Get_Legal_Moves(kingSquare, colour, KING, board);
         if(legalMoves) {return false;}

         for (const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN}) {
             U64 pieceBB = board.Get_Piece_Bitboard(piece, colour);

             while (pieceBB) {
                 int singlePiece = Get_LS1B_Index(pieceBB);
                 pieceBB &= pieceBB - 1;

                 const U64 legalPieceMoves = Moves_Generation.Get_Legal_Moves(singlePiece, colour, piece, board);
                 if(legalPieceMoves) {return false;}
             }
         }
         board.Set_Game_State(Game_State::CHECKMATE);
         return true;
}

    bool Handle_Castling(Move& move, Board& board) {

        if(!Can_Castle(move, board)) { return false;}

        const PieceColour colour = move.Get_Colour();
        const bool isKingSide = (move.Get_To() > move.Get_From());
        constexpr int whiteKingSideRook = h1;
        constexpr int whiteQueenSideRook = a1;
        constexpr int blackKingSideRook = h8;
        constexpr int blackQueenSideRook = a8;

        const int rookSquare = colour == WHITE ?
                              (isKingSide ? whiteKingSideRook : whiteQueenSideRook) :
                              (isKingSide ? blackKingSideRook : blackQueenSideRook);

        const int rookDestination = isKingSide ?
                                   (move.Get_To() - 1) :
                                   (move.Get_To() + 1);

        // Update Rook position
        U64 rookBitboard = board.Get_Piece_Bitboard(ROOK, colour);
        rookBitboard = Remove_Bit(rookBitboard, rookSquare);
        rookBitboard = Set_Bit(rookBitboard, rookDestination);
        Set_Piece_Bitboard(board, ROOK, colour, rookBitboard);

        // Update King position
        U64 kingBitboard = board.Get_Piece_Bitboard(KING, colour);
        kingBitboard = Remove_Bit(kingBitboard, move.Get_From());
        kingBitboard = Set_Bit(kingBitboard, move.Get_To());
        Set_Piece_Bitboard(board, KING, colour, kingBitboard);

        // Update castling flags by setting rook moves to maximum
        if(colour == WHITE) {
            board.castlingRights.whiteKingSideRookMoves = 9999;
            board.castlingRights.whiteQueenSideRookMoves = 9999;
        }
        else {
            board.castlingRights.blackKingSideRookMoves = 9999;
            board.castlingRights.blackQueenSideRookMoves = 9999;
        }

        move.Set_Castling(true);
        move.Set_King_Side_Castle(isKingSide);
        board.moveHistory.push_back(move);
        return true;
    }


    bool Can_Castle(const Move &move, const Board& board) {
        const PieceColour colour = move.Get_Colour();
        const PieceColour enemyColour = colour == WHITE ? BLACK : WHITE;
        const bool isKingSide = (move.Get_To() > move.Get_From());

        // Check king position
        const bool isKingInPosition = colour == WHITE ? (move.Get_From() == e1) : (move.Get_From() == e8);
        const bool isKingDestination = colour == WHITE ? ((move.Get_To() == c1) || (move.Get_To() == g1)) :
                                                        ((move.Get_To() == c8) || (move.Get_To() == g8));
        if(!isKingInPosition) {return false;}
        if (!isKingDestination) {return false;}

        // Check if king is in check
        if(Is_King_In_Check(board.Get_Piece_Bitboard(KING, colour), colour, board) != -1) {return false;}

        // Check castling rights and rook presence
        const int rookSquare = colour == WHITE ?
            (isKingSide ? h1 : a1) :
            (isKingSide ? h8 : a8);

        if(colour == WHITE) {
            if(isKingSide && board.castlingRights.whiteKingSideRookMoves > 0) { return false; }
            if(!isKingSide && board.castlingRights.whiteQueenSideRookMoves > 0) { return false; }
        }
        else {
            if(isKingSide && board.castlingRights.blackKingSideRookMoves > 0) { return false; }
            if(!isKingSide && board.castlingRights.blackQueenSideRookMoves > 0) { return false; }
        }

        // Verify rook is present
        if(!(board.Get_Piece_Bitboard(ROOK, colour) & (1ULL << rookSquare))) { return false; }

        // Check if squares between king and rook are empty
        const U64 occupancy = board.Get_All_Pieces();
        const U64 betweenSquares = Get_Path_Between(move.Get_From(), move.Get_To());
        if(betweenSquares & occupancy) { return false; }

        // Check if king passes through check on both squares
        const int square1 = move.Get_From() + (isKingSide ? 1 : -1);
        const int square2 = move.Get_From() + (isKingSide ? 2 : -2);

        if (Is_Square_Attacked(square1, enemyColour, board) != -1 ||
            Is_Square_Attacked(square2, enemyColour, board) != -1) { return false; }

        return true;
    }

    bool Is_Stalemate(const PieceColour colour, Board& board) {
        if(Is_King_In_Check(board.Get_Piece_Bitboard(KING, colour), colour, board)) { return false; }

        // Check each piece type
        for(const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            U64 pieceBitboard = board.Get_Piece_Bitboard(piece, colour);

            // Skip if no pieces of this type
            if(pieceBitboard == 0ULL) { continue; }

            // Check each individual piece of this type
            while(pieceBitboard) {
                const int pieceSquare = Get_LS1B_Index(pieceBitboard);

                const U64 legalMoves = Moves_Generation.Get_Legal_Moves(pieceSquare,
                                                                        colour,
                                                                        piece,
                                                                        board);

                // If any legal moves exist, not stalemate
                if(legalMoves) { return false; }

                // Move to next piece of same type
                pieceBitboard = Remove_Bit(pieceBitboard, pieceSquare);
            }
        }
        board.Set_Game_State(Game_State::STALEMATE);
        return true;  // No legal moves found for any piece
    }

    void Promote_Pawn(Move& move, const PieceType promotionPiece, Board& board) {

        const U64 enemyBB = move.Get_Colour() == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();
        // if (enemyBB & (1ULL << move.Get_To())){ Handle_Captures(move, board); }

        U64 newPieceBitboard = board.Get_Piece_Bitboard(promotionPiece, move.Get_Colour());
        newPieceBitboard = Set_Bit(newPieceBitboard, move.Get_To());
        Set_Piece_Bitboard(board, promotionPiece, move.Get_Colour(), newPieceBitboard);

        // Remove the pawn
        U64 pawnBitboard = board.Get_Piece_Bitboard(PAWN, move.Get_Colour());
        pawnBitboard = Remove_Bit(pawnBitboard, move.Get_From());
        Set_Piece_Bitboard(board, PAWN, move.Get_Colour(), pawnBitboard);

        move.Set_Promotion_Piece(promotionPiece);
    }

    void Move_Piece(const Move& move, Board& board) {
        PieceType piece = move.Get_Piece_Type();
        if (move.Get_Promotion_Piece() != NO_PIECE){ piece = move.Get_Promotion_Piece(); }

        U64 pieceBitboard = board.Get_Piece_Bitboard(piece, move.Get_Colour());
        pieceBitboard = Remove_Bit(pieceBitboard, move.Get_From());
        pieceBitboard = Set_Bit(pieceBitboard, move.Get_To());
        Set_Piece_Bitboard(board, piece, move.Get_Colour(), pieceBitboard);

        if (piece == ROOK) {
            if (move.Get_Colour() == WHITE) {
                if (move.Get_From() == a1) {
                    board.Increment_White_Queen_Side_Rook_Moves();
                }
                else if (move.Get_From() == h1) {
                    board.Increment_White_King_Side_Rook_Moves();
                }
            }
            else {
                if (move.Get_From() == a8) {
                    board.Increment_Black_Queen_Side_Rook_Moves();
                }
                else if (move.Get_From() == h8) {
                    board.Increment_Black_King_Side_Rook_Moves();

                }
            }
        }
        else if (piece == KING) {
            if (move.Get_Colour() == WHITE) {
                board.Increment_White_King_Side_Rook_Moves();
                board.Increment_White_Queen_Side_Rook_Moves();
            }
            else {
                board.Increment_Black_King_Side_Rook_Moves();
                board.Increment_Black_Queen_Side_Rook_Moves();
            }
        }

    }

    bool Can_Promote(const Move& move) {
        const PieceColour friendlyColour = move.Get_Colour();
        const bool whiteReached = friendlyColour == WHITE && move.Get_To() >= 0 && move.Get_To() <= 7;
        const bool blackReached = friendlyColour == BLACK && move.Get_To() >= 56 && move.Get_To() <= 63;

        if(whiteReached || blackReached) {
            return true;
        }
        return false;
    }

    void Handle_Captures(Move &move, Board &board) {
        const PieceColour enemyColour = move.Get_Colour() == WHITE ? BLACK : WHITE;
        const U64 destinationBit = 1ULL << move.Get_To();

        move.Set_Capture(true);

        // Find which enemy piece is being captured
        for(const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {

            if(destinationBit & board.Get_Piece_Bitboard(piece, enemyColour)) {

                move.Set_Captured_Piece(piece);
                move.Set_Capture_Destination(move.Get_To());

                U64 newBitboard = board.Get_Piece_Bitboard(piece, enemyColour);
                newBitboard = Remove_Bit(newBitboard, move.Get_To());
                Set_Piece_Bitboard(board, piece, enemyColour, newBitboard);
                break;
            }
        }
    }

    bool Can_En_Passant(const Move& move, const Board& board) {
        if(board.moveHistory.empty()) return false;

        const Move& previousMove = board.moveHistory.back();

        // Check if last move was pawn double push
        if(previousMove.Get_Piece_Type() != PAWN ||
           (abs(previousMove.Get_To() - previousMove.Get_From())) != 16) return false;

        // Check if capturing pawn is on correct rank (5th for white, 4th for black)
        const int fromRank = move.Get_From() / 8;
        if(move.Get_Colour() == WHITE && fromRank != 3) return false;  // 5th rank
        if(move.Get_Colour() == BLACK && fromRank != 4) return false;  // 4th rank

        // Check if files are adjacent
        const int moveFile = move.Get_From() % 8;
        const int captureFile = previousMove.Get_To() % 8;
        if(abs(moveFile - captureFile) != 1) return false;

        if (move.Get_Colour() == WHITE) {
            if (move.Get_To() + 8 != previousMove.Get_To()) { return false; }
        }
        else if (move.Get_Colour() == BLACK) {
            if (move.Get_To() - 8 != previousMove.Get_To()) { return false; }
        }

        // Check diagonal capture movement
        const int moveLength = move.Get_From() - move.Get_To();
        if(abs(moveLength) != 7 && abs(moveLength) != 9) return false;
        return true;
    }

    bool Handle_En_Passant(Move& move, Board& board) {
        if (!Can_En_Passant(move, board)){ return false; }

        // Remove captured pawn
        const PieceColour enemyColour = move.Get_Colour() == WHITE ? BLACK : WHITE;
        U64 enemyPawnBB = board.Get_Piece_Bitboard(PAWN, enemyColour);
        const int bitToRemove = move.Get_Colour() == WHITE ? 8 : -8;
        enemyPawnBB = Remove_Bit(enemyPawnBB, (bitToRemove + move.Get_To()));

        move.Set_Capture_Destination(static_cast<Squares>(bitToRemove + move.Get_To()));
        Set_Piece_Bitboard(board, PAWN, enemyColour, enemyPawnBB);

        // Move_Piece(move, board);

        move.Set_En_Passant(true);
        move.Set_Capture(true);
        move.Set_Captured_Piece(PAWN);
        // board.moveHistory.push_back(move);

        return true;
    }

    bool Make_Move(Move& move, const bool moveGeneration, Board& board) {
        const PieceType pieceType = move.Get_Piece_Type();
        const PieceColour pieceColour = move.Get_Colour();

        if (pieceType == NO_PIECE){ return false; }

        const U64 pieceLocation = 1ULL << move.Get_From();
        const U64 pieceDestination = 1ULL << move.Get_To();
        const U64 pieceBB = board.Get_Piece_Bitboard(pieceType, pieceColour);

        if (!(pieceBB & pieceLocation)){ return false; }

        const U64 pieceLegalMoves = Moves_Generation.Get_Legal_Moves(move.Get_From(), pieceColour, pieceType, board);

        if (!(pieceDestination & pieceLegalMoves)){ return false; }


        if (pieceType == PAWN) {
            Handle_En_Passant(move, board);
            if (Can_Promote(move)) {
                if (!moveGeneration) {
                    const PieceType promotionPiece = Choose_Piece_Promote();
                    Promote_Pawn(move,promotionPiece, board);
                }
                else { Promote_Pawn(move, move.Get_Promotion_Piece(), board); }
            }
        }

        else if (pieceType == KING) {
            if (abs(move.Get_To() - move.Get_From()) == 2) {
                if (Handle_Castling(move, board)) {
                    board.currentTurn = board.currentTurn == WHITE ? BLACK : WHITE;
                    return true;
                }
                return false;
            }
        }

        const PieceColour enemyColour = pieceColour == WHITE ? BLACK : WHITE;
        const U64 enemyPieces = pieceColour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();

        if (pieceDestination & enemyPieces){ Handle_Captures(move, board); }

        Move_Piece(move, board);

        Is_Checkmate(enemyColour, board);
        Is_Fifty_Move_Draw(board);

        Update_Half_Clock(move, board);
        board.moveHistory.push_back(move);
        board.currentTurn = board.currentTurn == WHITE ? BLACK : WHITE;
        return true;
    }

    bool Is_Insufficient_Material(const Board& board) {
            // Get piece counts for each side
            const int whitePawnCount = Count_Bits(board.Get_White_Pawn());
            const int whiteKnightCount = Count_Bits(board.Get_White_Knight());
            const int whiteBishopCount = Count_Bits(board.Get_White_Bishop());
            const int whiteRookCount = Count_Bits(board.Get_White_Rook());
            const int whiteQueenCount = Count_Bits(board.Get_White_Queen());

            const int blackPawnCount = Count_Bits(board.Get_Black_Pawn());
            const int blackKnightCount = Count_Bits(board.Get_Black_Knight());
            const int blackBishopCount = Count_Bits(board.Get_Black_Bishop());
            const int blackRookCount = Count_Bits(board.Get_Black_Rook());
            const int blackQueenCount = Count_Bits(board.Get_Black_Queen());

            if (whitePawnCount > 0 || blackPawnCount > 0 ||
                whiteRookCount > 0 || blackRookCount > 0 ||
                whiteQueenCount > 0 || blackQueenCount > 0) {
                return false;
            }

            if (whiteBishopCount == 0 && whiteKnightCount == 0 &&
                blackBishopCount == 0 && blackKnightCount == 0) {
                return true;
            }

            if ((whiteBishopCount == 0 && whiteKnightCount == 1 &&
                 blackBishopCount == 0 && blackKnightCount == 0) ||
                (whiteBishopCount == 0 && whiteKnightCount == 0 &&
                 blackBishopCount == 0 && blackKnightCount == 1)) {
                return true;
            }

            if ((whiteBishopCount == 1 && whiteKnightCount == 0 &&
                 blackBishopCount == 0 && blackKnightCount == 0) ||
                (whiteBishopCount == 0 && whiteKnightCount == 0 &&
                 blackBishopCount == 1 && blackKnightCount == 0)) {
                return true;
            }

            if (whiteBishopCount == 1 && whiteKnightCount == 0 &&
                blackBishopCount == 1 && blackKnightCount == 0) {
                const U64 whiteBishop = board.Get_White_Bishop();
                const U64 blackBishop = board.Get_Black_Bishop();

                const bool whiteSquare = (Get_LS1B_Index(whiteBishop) + (Get_LS1B_Index(whiteBishop) / 8)) % 2 == 0;
                const bool blackSquare = (Get_LS1B_Index(blackBishop) + (Get_LS1B_Index(blackBishop) / 8)) % 2 == 0;

                if (whiteSquare == blackSquare) {
                    return true;
                }
            }

            // All other cases have sufficient material
            return false;
        }

    void Update_Half_Clock(const Move &move, Board &board) {
        if(move.Get_Piece_Type() == PAWN || move.Is_Capture()){ board.Set_Reset_Half_Clock(); }

        else {board.Set_Increment_Half_Clock();}

    }

    bool Is_Fifty_Move_Draw(Board &board) {
        if(board.Get_Half_Clock() >= 100) {
            board.Set_Game_State(Game_State::FIFTY_MOVE_DRAW);
            return true;
        }
        return false;
    }

    void Set_Move_Flags(Move& move, const Board& board) {
        const U64 pieceDestination = 1ULL << move.Get_To();
        const PieceColour colour = move.Get_Colour();
        const PieceColour enemyColour = colour == WHITE ? BLACK : WHITE;
        const U64 enemyPieces = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();

        // Set capture flag
        if (pieceDestination & enemyPieces) {
            move.Set_Capture(true);
            move.Set_Captured_Piece(Find_Piece_Type(move.Get_To(), enemyColour, board));
            move.Set_Capture_Destination(move.Get_To());
        }

        // Set castling flag
        if (move.Get_Piece_Type() == KING && abs(move.Get_To() - move.Get_From()) == 2) {
            move.Set_Castling(true);
            move.Set_King_Side_Castle(move.Get_To() > move.Get_From());
        }

        // Set en passant flag
        if (move.Get_Piece_Type() == PAWN && Can_En_Passant(move, board)) {
            move.Set_En_Passant(true);
            move.Set_Capture(true);
            move.Set_Captured_Piece(PAWN);
        }
    }
}
