#pragma once
#include "../core/Bitboard_Operations.h"
#include "../Moves/Move.h"


enum class Game_State {
    ACTIVE,
    CHECK,
    CHECKMATE,
    STALEMATE,
    FIFTY_MOVE_DRAW,
    INSUFFICIENT_MATERIAL,
    THREEFOLD_REPETITION
};


class Board {
    std::array<U64,6> blackPieceArray;
    std::array<U64,6> whitePieceArray;
    std::array<U64, 4> rookTracking;     // black queen, black king, white queen, white king

    U64 whitePawn = 0ULL, whiteKnight = 0ULL, whiteBishop = 0ULL, whiteRook = 0ULL,
    whiteQueen = 0ULL, whiteKing = 0ULL;
    U64 blackPawn = 0ULL, blackKnight = 0ULL, blackBishop = 0ULL, blackRook = 0ULL,
    blackQueen = 0ULL, blackKing = 0ULL;


    U64 whitePieces = 0ULL;
    U64 blackPieces = 0ULL;
    U64 allPieces = 0ULL;

    void Update_Combined_Bitboards() {
        whitePieces = whitePawn | whiteKnight | whiteBishop | whiteRook | whiteQueen | whiteKing;
        blackPieces = blackPawn | blackKnight | blackBishop | blackRook | blackQueen | blackKing;
        allPieces = whitePieces | blackPieces;
    }

    Game_State boardGameState = Game_State::ACTIVE;

public:
    Board() {
        Update_Combined_Bitboards();
        blackPieceArray = {blackPawn, blackKnight, blackBishop, blackRook, blackQueen, blackKing};
        whitePieceArray = {whitePawn, whiteKnight, whiteBishop, whiteRook, whiteQueen, whiteKing};
    }
    struct CastlingRights {
        int whiteKingSideRookMoves = 0;
        int whiteQueenSideRookMoves = 0;
        int blackKingSideRookMoves = 0;
        int blackQueenSideRookMoves = 0;
    } castlingRights;

    std::vector<Move> moveHistory;
    PieceColour currentTurn = WHITE;
    int Half_Clock = 0;

    static void Print_Move_Details(const Move& move) {
        std::cout << "\nMove Details:\n"
                  << "From: " << Square_To_String(move.Get_From())
                  << " To: " << Square_To_String(move.Get_To())
                  << "\nPiece: " << (move.Get_Colour() == WHITE ? "White " : "Black ") << Get_Piece_Name(move.Get_Piece_Type());

        if(move.Is_Capture()) std::cout << "\nCaptures: " << Get_Piece_Name(move.Get_Captured_Piece());
        if(move.Is_Castling()) std::cout << "\nCastling: " << (move.Get_Castle_Side() ? "Kingside" : "Queenside");
        if(move.Is_En_Passant()) std::cout << "\nEn Passant Capture";
        if(move.Get_Promotion_Piece() != NO_PIECE) std::cout << "\nPromotion to: " << Get_Piece_Name(move.Get_Promotion_Piece());
        std::cout << "\n";
    }

    void Undo_Move(const bool pseudoLegal) {
        if(moveHistory.empty()) { return; }

        Move previousMove = moveHistory.back();
        moveHistory.pop_back();

        if(previousMove.Is_Castling()) {
            Reverse_Castle(previousMove);
            if(!pseudoLegal) {Set_Decrement_Half_Clock();}
            return;
        }

        Reverse_Promotion(previousMove);
        Reverse_Capture(previousMove);


        if (previousMove.Get_Moved_Rook() != NOT_ROOK) {
            switch (previousMove.Get_Moved_Rook()) {
                case WHITE_KING_SIDE:
                    Decrement_White_King_Side_Rook_Moves();
                break;
                case WHITE_QUEEN_SIDE:
                    Decrement_White_Queen_Side_Rook_Moves();
                break;
                case BLACK_KING_SIDE:
                    Decrement_Black_King_Side_Rook_Moves();
                break;
                case BLACK_QUEEN_SIDE:
                    Decrement_Black_Queen_Side_Rook_Moves();
                break;
                default:
                    break;
            }
        }
        if (previousMove.Get_Piece_Type() == KING) {
            if (previousMove.Get_Colour() == WHITE) {
                Decrement_White_King_Side_Rook_Moves();
                Decrement_White_Queen_Side_Rook_Moves();
            }
            else {
                Decrement_Black_King_Side_Rook_Moves();
                Decrement_Black_Queen_Side_Rook_Moves();
            }
        }

        const PieceColour pieceColour = previousMove.Get_Colour();
        const PieceType piece = previousMove.Get_Piece_Type();

        U64 pieceBB = Get_Piece_Bitboard(piece, pieceColour);
        pieceBB = Remove_Bit(pieceBB, previousMove.Get_To());
        pieceBB = Set_Bit(pieceBB, previousMove.Get_From());

        Set_Piece_Bitboard(piece, pieceColour, pieceBB);

        if(!pseudoLegal) {Set_Decrement_Half_Clock();}
   }

    void Reverse_Castle(const Move& move) {
        const PieceColour colour = move.Get_Colour();
        U64 kingBB = Get_Piece_Bitboard(KING, colour);
        U64 rookBB = Get_Piece_Bitboard(ROOK, colour);

        kingBB = Remove_Bit(kingBB, move.Get_To());
        kingBB = Set_Bit(kingBB, move.Get_From());
        const int squareToRemove = move.Get_Castle_Side() ? (colour == WHITE ? f1 : f8) : (colour == WHITE ? d1 : d8);
        const int squareToRecover = move.Get_Castle_Side() ? (colour == WHITE ? h1 : h8) : (colour == WHITE ? a1 : a8);

        rookBB = Remove_Bit(rookBB, squareToRemove);
        rookBB = Set_Bit(rookBB, squareToRecover);

        Set_Piece_Bitboard(KING, colour, kingBB);
        Set_Piece_Bitboard(ROOK, colour, rookBB);

        if (colour == WHITE) {
            castlingRights.whiteKingSideRookMoves = 0;
            castlingRights.whiteQueenSideRookMoves = 0;
        }
        else {
            castlingRights.blackKingSideRookMoves = 0;
            castlingRights.blackQueenSideRookMoves = 0;
        }
    }

    void Reverse_Promotion(const Move& move) {

        if (move.Get_Promotion_Piece() == NO_PIECE) {return; }
        const PieceColour colour = move.Get_Colour();
        const PieceType promotedPieceType = move.Get_Promotion_Piece();

        U64 promotedPieceBB = Get_Piece_Bitboard(promotedPieceType, colour);
        promotedPieceBB = Remove_Bit(promotedPieceBB, move.Get_To());
        Set_Piece_Bitboard(promotedPieceType, colour, promotedPieceBB);

    }

    void Reverse_Capture(const Move &move) {

        if (!move.Is_Capture()) {return ; }
        const PieceColour colour = move.Get_Colour();
        const PieceColour enemyColour = colour == WHITE ? BLACK : WHITE;
        const PieceType capturedPiece = move.Get_Captured_Piece();

        U64 capturedPieceBB = Get_Piece_Bitboard(capturedPiece, enemyColour);
        capturedPieceBB = Set_Bit(capturedPieceBB, move.Get_Captured_Destination());
        Set_Piece_Bitboard(capturedPiece, enemyColour, capturedPieceBB);
    }


    std::string Board_To_Fen() const {
        std::string fen;
        int emptyCount = 0;

        // Loop through board ranks (8->1)
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                char pieceChar = 0;  // Use 0 instead of '.' for empty squares

                // Check each piece type/color
                if (Get_Bit(whitePieceArray[0], square)) pieceChar = 'P';
                else if (Get_Bit(whitePieceArray[1], square)) pieceChar = 'N';
                else if (Get_Bit(whitePieceArray[2], square)) pieceChar = 'B';
                else if (Get_Bit(whitePieceArray[3], square)) pieceChar = 'R';
                else if (Get_Bit(whitePieceArray[4], square)) pieceChar = 'Q';
                else if (Get_Bit(whitePieceArray[5], square)) pieceChar = 'K';
                else if (Get_Bit(blackPieceArray[0], square)) pieceChar = 'p';
                else if (Get_Bit(blackPieceArray[1], square)) pieceChar = 'n';
                else if (Get_Bit(blackPieceArray[2], square)) pieceChar = 'b';
                else if (Get_Bit(blackPieceArray[3], square)) pieceChar = 'r';
                else if (Get_Bit(blackPieceArray[4], square)) pieceChar = 'q';
                else if (Get_Bit(blackPieceArray[5], square)) pieceChar = 'k';

                if (pieceChar == 0) {
                    emptyCount++;
                } else {
                    if (emptyCount > 0) {
                        fen += std::to_string(emptyCount);
                        emptyCount = 0;
                    }
                    fen += pieceChar;
                }
            }

            if (emptyCount > 0) {
                fen += std::to_string(emptyCount);
                emptyCount = 0;
            }

            if (rank < 7) {
                fen += '/';
            }
        }

        // Add turn
        fen += currentTurn == WHITE ? " w " : " b ";

        // Add castling rights
        std::string castling;
        if (Has_White_King_Side_Castling_Rights()) castling += 'K';
        if (Has_White_Queen_Side_Castling_Rights()) castling += 'Q';
        if (Has_Black_King_Side_Castling_Rights()) castling += 'k';
        if (Has_Black_Queen_Side_Castling_Rights()) castling += 'q';

        fen += castling.empty() ? "-" : castling;

        // Add en passant square, halfmove clock, and fullmove number
        fen += " - " + std::to_string(Half_Clock) + " 1";

        return fen;
    }

    void Initialise_From_Fen(const std::string& fen) {

        whitePawn = Parse_Fen(fen, true, PAWN);
        whiteKnight = Parse_Fen(fen, true, KNIGHT);
        whiteBishop = Parse_Fen(fen, true, BISHOP);
        whiteRook = Parse_Fen(fen, true, ROOK);
        whiteQueen = Parse_Fen(fen, true, QUEEN);
        whiteKing = Parse_Fen(fen, true, KING);

        blackPawn = Parse_Fen(fen, false, PAWN);
        blackKnight = Parse_Fen(fen, false, KNIGHT);
        blackBishop = Parse_Fen(fen, false, BISHOP);
        blackRook = Parse_Fen(fen, false, ROOK);
        blackQueen = Parse_Fen(fen, false, QUEEN);
        blackKing = Parse_Fen(fen, false, KING);


        size_t fenPos = fen.find(' ');
        char turn = fen[fenPos + 1];
        currentTurn = (turn == 'w') ? WHITE : BLACK;


        size_t pos = fen.find(' ');
        pos = fen.find(' ', pos + 1);
        std::string castling = fen.substr(pos + 1, fen.find(' ', pos + 1) - pos - 1);

        castlingRights.whiteKingSideRookMoves = 9999;
        castlingRights.whiteQueenSideRookMoves = 9999;
        castlingRights.blackKingSideRookMoves = 9999;
        castlingRights.blackQueenSideRookMoves = 9999;

        for(char c : castling) {
            switch(c) {
                case 'K':
                    castlingRights.whiteKingSideRookMoves = 0;

                break;
                case 'Q':
                    castlingRights.whiteQueenSideRookMoves = 0;
                break;
                case 'k':
                    castlingRights.blackKingSideRookMoves = 0;
                break;
                case 'q':
                    castlingRights.blackQueenSideRookMoves = 0;
                break;
            }
        }

        whitePieceArray = {whitePawn, whiteKnight, whiteBishop, whiteRook, whiteQueen, whiteKing};
        blackPieceArray = {blackPawn, blackKnight, blackBishop,blackRook, blackQueen, blackKing};

        Update_Combined_Bitboards();
    }

    [[nodiscard]] Game_State Get_Game_State() const { return boardGameState; }
    [[nodiscard]] int Get_Half_Clock() const { return Half_Clock; }

    [[nodiscard]] U64 Get_White_Pawn() const { return whitePawn; }
    [[nodiscard]] U64 Get_White_Knight() const { return whiteKnight; }
    [[nodiscard]] U64 Get_White_Bishop() const { return whiteBishop; }
    [[nodiscard]] U64 Get_White_Rook() const { return whiteRook; }
    [[nodiscard]] U64 Get_White_Queen() const { return whiteQueen; }
    [[nodiscard]] U64 Get_White_King() const { return whiteKing; }

    [[nodiscard]] U64 Get_Black_Pawn() const { return blackPawn; }
    [[nodiscard]] U64 Get_Black_Knight() const { return blackKnight; }
    [[nodiscard]] U64 Get_Black_Bishop() const { return blackBishop; }
    [[nodiscard]] U64 Get_Black_Rook() const { return blackRook; }
    [[nodiscard]] U64 Get_Black_Queen() const { return blackQueen; }
    [[nodiscard]] U64 Get_Black_King() const { return blackKing; }

    [[nodiscard]] U64 Get_White_Pieces() const { return whitePieces; }
    [[nodiscard]] U64 Get_Black_Pieces() const { return blackPieces; }
    [[nodiscard]] U64 Get_All_Pieces() const { return allPieces; }

    [[nodiscard]] bool Has_White_King_Side_Castling_Rights() const { return castlingRights.whiteKingSideRookMoves == 0; }
    void Increment_White_King_Side_Rook_Moves() { castlingRights.whiteKingSideRookMoves++; }
    void Decrement_White_King_Side_Rook_Moves() { castlingRights.whiteKingSideRookMoves--; }

    [[nodiscard]] bool Has_White_Queen_Side_Castling_Rights() const { return castlingRights.whiteQueenSideRookMoves == 0; }
    void Increment_White_Queen_Side_Rook_Moves() { castlingRights.whiteQueenSideRookMoves++; }
    void Decrement_White_Queen_Side_Rook_Moves() { castlingRights.whiteQueenSideRookMoves--; }

    [[nodiscard]] bool Has_Black_King_Side_Castling_Rights() const { return castlingRights.blackKingSideRookMoves == 0; }
    void Increment_Black_King_Side_Rook_Moves() { castlingRights.blackKingSideRookMoves++; }
    void Decrement_Black_King_Side_Rook_Moves() { castlingRights.blackKingSideRookMoves--; }

    [[nodiscard]] bool Has_Black_Queen_Side_Castling_Rights() const { return castlingRights.blackQueenSideRookMoves == 0; }
    void Increment_Black_Queen_Side_Rook_Moves() { castlingRights.blackQueenSideRookMoves++; }
    void Decrement_Black_Queen_Side_Rook_Moves() { castlingRights.blackQueenSideRookMoves--; }

    void Set_Increment_Half_Clock() { Half_Clock++; }
    void Set_Decrement_Half_Clock() { Half_Clock--; }
    void Set_Reset_Half_Clock() { Half_Clock = 0; }

    void Set_Game_State(const Game_State state) { boardGameState = state; }

    void Set_White_Pawn(const U64 bitboard) {
        whitePawn = bitboard;
        whitePieceArray[PAWN] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_White_Knight(const U64 bitboard) {
        whiteKnight = bitboard;
        whitePieceArray[KNIGHT] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_White_Bishop(const U64 bitboard) {
        whiteBishop = bitboard;
        whitePieceArray[BISHOP] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_White_Rook(const U64 bitboard) {
        whiteRook = bitboard;
        whitePieceArray[ROOK] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_White_Queen(const U64 bitboard) {
        whiteQueen = bitboard;
        whitePieceArray[QUEEN] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_White_King(const U64 bitboard) {
        whiteKing = bitboard;
        whitePieceArray[KING] = bitboard;
        Update_Combined_Bitboards();
    }

    void Set_Black_Pawn(const U64 bitboard) {
        blackPawn = bitboard;
        blackPieceArray[PAWN] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_Black_Knight(const U64 bitboard) {
        blackKnight = bitboard;
        blackPieceArray[KNIGHT] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_Black_Bishop(const U64 bitboard) {
        blackBishop = bitboard;
        blackPieceArray[BISHOP] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_Black_Rook(const U64 bitboard) {
        blackRook = bitboard;
        blackPieceArray[ROOK] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_Black_Queen(const U64 bitboard) {
        blackQueen = bitboard;
        blackPieceArray[QUEEN] = bitboard;
        Update_Combined_Bitboards();
    }
    void Set_Black_King(const U64 bitboard) {
        blackKing = bitboard;
        blackPieceArray[KING] = bitboard;
        Update_Combined_Bitboards();
    }


    void Set_Piece_Bitboard(const PieceType piece, const PieceColour colour, const U64 bitboard) {
        if(colour == WHITE) {
            switch(piece) {
                case PAWN:
                    Set_White_Pawn(bitboard);
                    break;
                case ROOK:
                    Set_White_Rook(bitboard);
                    break;
                case KNIGHT:
                    Set_White_Knight(bitboard);
                    break;
                case BISHOP:
                    Set_White_Bishop(bitboard);
                    break;
                case QUEEN:
                    Set_White_Queen(bitboard);
                    break;
                case KING:
                    Set_White_King(bitboard);
                    break;
                default:
                    break;
            }
        } else {
            switch(piece) {
                case PAWN:
                    Set_Black_Pawn(bitboard);
                    break;
                case ROOK:
                    Set_Black_Rook(bitboard);
                    break;
                case KNIGHT:
                    Set_Black_Knight(bitboard);
                    break;
                case BISHOP:
                    Set_Black_Bishop(bitboard);
                    break;
                case QUEEN:
                    Set_Black_Queen(bitboard);
                    break;
                case KING:
                    Set_Black_King(bitboard);
                    break;
                default:
                    break;
            }
        }
    }

    [[nodiscard]] U64 Get_Piece_Bitboard(const PieceType piece, const PieceColour colour) const {
        if(colour == WHITE) {
            switch(piece) {
                case PAWN: return Get_White_Pawn();
                case ROOK: return Get_White_Rook();
                case KNIGHT: return Get_White_Knight();
                case BISHOP: return Get_White_Bishop();
                case QUEEN: return Get_White_Queen();
                case KING: return Get_White_King();
                default: return 0ULL;
            }
        }
        switch(piece) {
            case PAWN: return Get_Black_Pawn();
            case ROOK: return Get_Black_Rook();
            case KNIGHT: return Get_Black_Knight();
            case BISHOP: return Get_Black_Bishop();
            case QUEEN: return Get_Black_Queen();
            case KING: return Get_Black_King();
            default: return 0ULL;
        }

    }

    void Print_Detailed_Board()  const {
        int count = 8;
        for(int file = 0; file < 8; file++) {
            std::cout << count << "  ";

            for(int rank = 0; rank < 8; rank++) {
                int position = file * 8 + rank;
                char pieceChar = '.';

                if(Get_Bit(whitePieceArray[0], position)) pieceChar = 'P';
                else if(Get_Bit(whitePieceArray[1], position)) pieceChar = 'N';
                else if(Get_Bit(whitePieceArray[2], position)) pieceChar = 'B';
                else if(Get_Bit(whitePieceArray[3], position)) pieceChar = 'R';
                else if(Get_Bit(whitePieceArray[4], position)) pieceChar = 'Q';
                else if(Get_Bit(whitePieceArray[5], position)) pieceChar = 'K';

                else if(Get_Bit(blackPieceArray[0], position)) pieceChar = 'p';
                else if(Get_Bit(blackPieceArray[1], position)) pieceChar = 'n';
                else if(Get_Bit(blackPieceArray[2], position)) pieceChar = 'b';
                else if(Get_Bit(blackPieceArray[3], position)) pieceChar = 'r';
                else if(Get_Bit(blackPieceArray[4], position)) pieceChar = 'q';
                else if(Get_Bit(blackPieceArray[5], position)) pieceChar = 'k';

                std::cout << pieceChar << " ";
            }
            std::cout << std::endl;
            count -= 1;
        }
        std::cout << "   a b c d e f g h" << std::endl;
        std::cout << std::endl;
    }


};
