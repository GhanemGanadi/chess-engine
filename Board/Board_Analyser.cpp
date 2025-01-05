#include "../Board/Board_Analyser.h"

#include <filesystem>
#include "../Moves/Move_Generation.h"

namespace Board_Analyser {
    auto Moves_Generation = MoveGeneration();

    int Is_Square_Attacked(const int square, const PieceColour attackerColour, const Board& board) {

        const U64 pawnAttacks = Moves_Generation.Get_Pawn_Attacks(square, attackerColour == WHITE ? BLACK : WHITE);
        if (pawnAttacks & board.Get_Piece_Bitboard(PAWN, attackerColour)) {
            return Get_Square(pawnAttacks & board.Get_Piece_Bitboard(PAWN, attackerColour));
        }

        U64 knightAttacks = Moves_Generation.Get_Knight_Moves(square);
        if (knightAttacks & board.Get_Piece_Bitboard(KNIGHT, attackerColour)) {
            return Get_Square(knightAttacks & board.Get_Piece_Bitboard(KNIGHT, attackerColour));
        }

        U64 kingAttacks = MoveGeneration::Get_King_Moves(square);
        if (kingAttacks & board.Get_Piece_Bitboard(KING, attackerColour)){
            return Get_Square(kingAttacks & board.Get_Piece_Bitboard(KING, attackerColour));
        }

        U64 occupancy = board.Get_All_Pieces();

        U64 rookAttacks = Tables::Get_Rook_Moves(square, occupancy);
        if (rookAttacks & (board.Get_Piece_Bitboard(ROOK, attackerColour))) {
            return Get_Square(rookAttacks & board.Get_Piece_Bitboard(ROOK, attackerColour));
        }

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

        U64 rookBitboard = board.Get_Piece_Bitboard(ROOK, colour);
        rookBitboard = Remove_Bit(rookBitboard, rookSquare);
        rookBitboard = Set_Bit(rookBitboard, rookDestination);
        Set_Piece_Bitboard(board, ROOK, colour, rookBitboard);

        U64 kingBitboard = board.Get_Piece_Bitboard(KING, colour);
        kingBitboard = Remove_Bit(kingBitboard, move.Get_From());
        kingBitboard = Set_Bit(kingBitboard, move.Get_To());
        Set_Piece_Bitboard(board, KING, colour, kingBitboard);

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
        board.castlingRightsHistory.push_back(board.castlingRights);
        return true;
    }


    bool Can_Castle(const Move &move, const Board& board) {
        const PieceColour colour = move.Get_Colour();
        const PieceColour enemyColour = colour == WHITE ? BLACK : WHITE;
        const bool isKingSide = (move.Get_To() > move.Get_From());

        const bool isKingInPosition = colour == WHITE ? (move.Get_From() == e1) : (move.Get_From() == e8);
        const bool isKingDestination = colour == WHITE ? ((move.Get_To() == c1) || (move.Get_To() == g1)) :
                                                        ((move.Get_To() == c8) || (move.Get_To() == g8));
        if(!isKingInPosition) {return false;}
        if (!isKingDestination) {return false;}

        if(Is_King_In_Check(board.Get_Piece_Bitboard(KING, colour), colour, board) != -1) {return false;}

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

        if(!(board.Get_Piece_Bitboard(ROOK, colour) & (1ULL << rookSquare))) { return false; }

        const U64 occupancy = board.Get_All_Pieces();
        const U64 betweenSquares = Get_Path_Between(move.Get_From(), move.Get_To());
        if(betweenSquares & occupancy) { return false; }

        const int square1 = move.Get_From() + (isKingSide ? 1 : -1);
        const int square2 = move.Get_From() + (isKingSide ? 2 : -2);

        if (Is_Square_Attacked(square1, enemyColour, board) != -1 ||
            Is_Square_Attacked(square2, enemyColour, board) != -1) { return false; }

        return true;
    }

    bool Is_Stalemate(const PieceColour colour, Board& board) {
        if(Is_King_In_Check(board.Get_Piece_Bitboard(KING, colour), colour, board)) { return false; }

        for(const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            U64 pieceBitboard = board.Get_Piece_Bitboard(piece, colour);

            if(pieceBitboard == 0ULL) { continue; }

            while(pieceBitboard) {
                const int pieceSquare = Get_LS1B_Index(pieceBitboard);

                const U64 legalMoves = Moves_Generation.Get_Legal_Moves(pieceSquare,
                                                                        colour,
                                                                        piece,
                                                                        board);

                if(legalMoves) { return false; }

                pieceBitboard = Remove_Bit(pieceBitboard, pieceSquare);
            }
        }
        board.Set_Game_State(Game_State::STALEMATE);
        return true;
    }

    void Promote_Pawn(Move& move, const PieceType promotionPiece, Board& board) {

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
        const U64 fromBB = 1ULL << move.Get_From();
        const U64 toBit = 1ULL << move.Get_To();

        PieceType piece = move.Get_Piece_Type();
        if (move.Get_Promotion_Piece() != NO_PIECE){ piece = move.Get_Promotion_Piece(); }

        U64 pieceBitboard = board.Get_Piece_Bitboard(piece, move.Get_Colour());
        pieceBitboard = Remove_Bit(pieceBitboard, move.Get_From());
        pieceBitboard = Set_Bit(pieceBitboard, move.Get_To());
        Set_Piece_Bitboard(board, piece, move.Get_Colour(), pieceBitboard);

        if (piece == ROOK) {
            if (fromBB & board.whiteKingSideRook) {
                board.whiteKingSideRook = toBit;
                board.Increment_White_King_Side_Rook_Moves();
            }
            else if (fromBB & board.whiteQueenSideRook) {
                board.whiteQueenSideRook = toBit;
                board.Increment_White_Queen_Side_Rook_Moves();
            }
            else if (fromBB & board.blackKingSideRook) {
                board.blackKingSideRook = toBit;
                board.Increment_Black_King_Side_Rook_Moves();
            }
            else if (fromBB & board.blackQueenSideRook) {
                board.blackQueenSideRook = toBit;
                board.Increment_Black_Queen_Side_Rook_Moves();
            }
            return;
        }

        if (piece == KING) {
            if (move.Get_Colour() == WHITE) {
                board.Increment_White_King_Side_Rook_Moves();
                board.Increment_White_Queen_Side_Rook_Moves();
                return;
            }
            board.Increment_Black_King_Side_Rook_Moves();
            board.Increment_Black_Queen_Side_Rook_Moves();
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

        if(previousMove.Get_Piece_Type() != PAWN ||
           (abs(previousMove.Get_To() - previousMove.Get_From())) != 16) return false;

        const int fromRank = move.Get_From() / 8;
        if(move.Get_Colour() == WHITE && fromRank != 3) return false;  // 5th rank
        if(move.Get_Colour() == BLACK && fromRank != 4) return false;  // 4th rank

        const int moveFile = move.Get_From() % 8;
        const int captureFile = previousMove.Get_To() % 8;
        if(abs(moveFile - captureFile) != 1) return false;

        if (move.Get_Colour() == WHITE) {
            if (move.Get_To() + 8 != previousMove.Get_To()) { return false; }
        }
        else if (move.Get_Colour() == BLACK) {
            if (move.Get_To() - 8 != previousMove.Get_To()) { return false; }
        }

        const int moveLength = move.Get_From() - move.Get_To();
        if(abs(moveLength) != 7 && abs(moveLength) != 9) return false;
        return true;
    }

    bool Handle_En_Passant(Move& move, Board& board) {
        if (!Can_En_Passant(move, board)){ return false; }

        const PieceColour enemyColour = move.Get_Colour() == WHITE ? BLACK : WHITE;
        U64 enemyPawnBB = board.Get_Piece_Bitboard(PAWN, enemyColour);
        const int bitToRemove = move.Get_Colour() == WHITE ? 8 : -8;
        enemyPawnBB = Remove_Bit(enemyPawnBB, (bitToRemove + move.Get_To()));

        move.Set_Capture_Destination(static_cast<Squares>(bitToRemove + move.Get_To()));
        Set_Piece_Bitboard(board, PAWN, enemyColour, enemyPawnBB);

        move.Set_En_Passant(true);
        move.Set_Capture(true);
        move.Set_Captured_Piece(PAWN);

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
        board.castlingRightsHistory.push_back(board.castlingRights);
        board.currentTurn = board.currentTurn == WHITE ? BLACK : WHITE;
        return true;
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

        if (pieceDestination & enemyPieces) {
            move.Set_Capture(true);
            move.Set_Captured_Piece(Find_Piece_Type(move.Get_To(), enemyColour, board));
            move.Set_Capture_Destination(move.Get_To());
        }

        if (move.Get_Piece_Type() == KING && abs(move.Get_To() - move.Get_From()) == 2) {
            move.Set_Castling(true);
            move.Set_King_Side_Castle(move.Get_To() > move.Get_From());
        }

        if (move.Get_Piece_Type() == PAWN && Can_En_Passant(move, board)) {
            move.Set_En_Passant(true);
            move.Set_Capture(true);
            move.Set_Captured_Piece(PAWN);
        }
    }
}
