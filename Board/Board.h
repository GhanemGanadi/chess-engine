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
    int Half_Clock = 0;
    Game_State boardGameState = Game_State::ACTIVE;

    std::array<U64,6> blackPieceArray;
    std::array<U64,6> whitePieceArray;

    U64 whitePawn = 0ULL, whiteKnight = 0ULL, whiteBishop = 0ULL,
        whiteRook = 0ULL, whiteQueen = 0ULL, whiteKing = 0ULL;

    U64 blackPawn = 0ULL, blackKnight = 0ULL, blackBishop = 0ULL,
        blackRook = 0ULL, blackQueen = 0ULL, blackKing = 0ULL;

    U64 whitePieces = 0ULL;
    U64 blackPieces = 0ULL;



    static constexpr U64 whiteQueenSideMask = 1ULL << b1 | 1ULL << c1 | 1ULL << d1;
    static constexpr U64 whiteKingSideMask = 1ULL << f1 | 1ULL << g1;

    static constexpr U64 blackKingSideMask = 1ULL << f8 | 1ULL << g8;
    static constexpr U64 blackQueenSideMask = 1ULL << b8 | 1ULL << c8 | 1ULL << d8;

    void Update_Combined_Bitboards(const bool updateArray) {
        whitePieces = whitePawn | whiteKnight | whiteBishop | whiteRook | whiteQueen | whiteKing;
        blackPieces = blackPawn | blackKnight | blackBishop | blackRook | blackQueen | blackKing;
        if (updateArray) {
            blackPieceArray = {blackPawn, blackKnight, blackBishop, blackRook, blackQueen, blackKing};
            whitePieceArray = {whitePawn, whiteKnight, whiteBishop, whiteRook, whiteQueen, whiteKing};
        }
    }


public:
    U64 whiteKingRook = 0ULL;
    U64 whiteQueenRook = 0ULL;
    U64 blackKingRook = 0ULL;
    U64 blackQueenRook = 0ULL;

    Board() {
        Update_Combined_Bitboards(true);
    }
    struct CastlingRights {
        int whiteKingSideRookMoves = 0;
        int whiteQueenSideRookMoves = 0;
        int blackKingSideRookMoves = 0;
        int blackQueenSideRookMoves = 0;
    } castlingRights;

    std::vector<Move> moveHistory;
    PieceColour currentTurn = WHITE;

    void Undo_Move(const bool pseudoLegal) {
        if (moveHistory.empty()) { return; }
        currentTurn = currentTurn == WHITE ? BLACK : WHITE;

        const Move previousMove = moveHistory.back();
        moveHistory.pop_back();

        const PieceColour pieceColour = previousMove.Get_Colour();
        const PieceType piece = previousMove.Get_Piece_Type();

        if(previousMove.Is_Castling()) {
            Reverse_Castle(previousMove);
            if(!pseudoLegal) {Adjust_Half_Clock(false); }
            return;
        }

        Reverse_Promotion(previousMove);
        Reverse_Capture(previousMove);

        if (piece == KING || piece == ROOK) { Adjust_Castling_Flags(previousMove, true); }


        U64 pieceBB = Get_Piece_Bitboard(piece, pieceColour, previousMove.Get_Castle_Side());
        pieceBB = Remove_Bit(pieceBB, previousMove.Get_To());
        pieceBB = Set_Bit(pieceBB, previousMove.Get_From());

        Set_Piece_Bitboard(piece, pieceColour, pieceBB, previousMove.Get_Castle_Side());

        if(!pseudoLegal) {Adjust_Half_Clock(false); }
   }

    void Reverse_Castle(const Move& move) {
        const PieceColour colour = move.Get_Colour();

        const int squareToRecover = move.Get_Castle_Side() == Castle_Side::King_Side ?
                                                            (colour == WHITE ? h1 : h8) :
                                                            (colour == WHITE ? a1 : a8);
        const U64 rookBB = 1ULL << squareToRecover;
        const U64 kingBB = 1ULL << move.Get_From();

        Set_Piece_Bitboard(KING, colour, kingBB);
        Set_Piece_Bitboard(ROOK, colour, rookBB, move.Get_Castle_Side());

        Set_Castling_Flag(colour, true);
    }

    void Reverse_Promotion(const Move& move) {
        if (move.Get_Promotion_Piece() == NO_PIECE) {return; }

        const PieceColour colour = move.Get_Colour();
        const PieceType promotedPieceType = move.Get_Promotion_Piece();

        U64 promotedPieceBB = Get_Piece_Bitboard(promotedPieceType, colour);
        promotedPieceBB = Remove_Bit(promotedPieceBB, move.Get_To());
        Set_Piece_Bitboard(promotedPieceType, colour, promotedPieceBB, move.Get_Castle_Side());
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

    void Set_Piece_Bitboard(const PieceType piece, const PieceColour colour, U64 bitboard,
                            const std::optional<Castle_Side> kingSide=std::nullopt) {
        if(colour == WHITE) {
            switch(piece) {
                case PAWN: {
                    whitePawn = bitboard;
                    whitePieceArray[PAWN] = bitboard;
                    break;
                }
                case KNIGHT: {
                    whiteKnight = bitboard;
                    whitePieceArray[KNIGHT] = bitboard;
                    break;
                }
                case BISHOP: {
                    whiteBishop = bitboard;
                    whitePieceArray[BISHOP] = bitboard;
                    break;
                }
                case ROOK: {
                    if (kingSide.has_value()) {
                        if (kingSide == Castle_Side::King_Side) {
                            whiteRook ^= whiteKingRook;
                            whiteKingRook = bitboard;
                            whiteRook |= whiteKingRook;
                        }
                        else if (kingSide == Castle_Side::Queen_Side) {
                            whiteRook ^= whiteQueenRook;
                            whiteQueenRook = bitboard;
                            whiteRook |= whiteQueenRook;
                        }
                        else { whiteRook = bitboard; }
                    }
                    else { whiteRook = bitboard; }
                    whitePieceArray[ROOK] = whiteRook;
                    break;
                }
                case QUEEN: {
                    whiteQueen = bitboard;
                    whitePieceArray[QUEEN] = bitboard;
                    break;
                }
                case KING:{
                    whiteKing = bitboard;
                    whitePieceArray[KING] = bitboard;
                    break;
                }
                default:
                    break;
            }
        }
        else {
            switch(piece) {

                case PAWN: {
                    blackPawn = bitboard;
                    blackPieceArray[PAWN] = bitboard;
                    break;
                }
                case KNIGHT: {
                    blackKnight = bitboard;
                    blackPieceArray[KNIGHT] = bitboard;
                    break;
                }
                case BISHOP: {
                    blackBishop = bitboard;
                    blackPieceArray[BISHOP] = bitboard;
                    break;
                }
                case ROOK: {
                    if (kingSide.has_value()) {
                        if (kingSide == Castle_Side::King_Side) {
                            blackRook ^= blackKingRook;
                            blackKingRook = bitboard;
                            blackRook |= blackKingRook;
                        }
                        else if (kingSide == Castle_Side::Queen_Side) {
                            blackRook ^= blackQueenRook;
                            blackQueenRook = bitboard;
                            blackRook |= blackQueenRook;
                        }
                        else { blackRook = bitboard; }

                    }
                    else { blackRook = bitboard; }
                    blackPieceArray[ROOK] = blackRook;
                    break;
                }
                case QUEEN: {
                    blackQueen = bitboard;
                    blackPieceArray[QUEEN] = bitboard;
                    break;
                }
                case KING: {
                    blackKing = bitboard;
                    blackPieceArray[KING] = bitboard;
                    break;
                }
                default:
                    break;
            }
        }
        Update_Combined_Bitboards(false);
    }

    [[nodiscard]] U64 Get_Piece_Bitboard(const PieceType piece, const PieceColour colour,
                                        const std::optional<Castle_Side> kingSide=std::nullopt) const {
        if(colour == WHITE) {
            switch(piece) {
                case PAWN: return whitePawn;
                case ROOK:{
                    if (kingSide.has_value()) {
                        if (kingSide == Castle_Side::King_Side) {return whiteKingRook;}
                        if (kingSide == Castle_Side::Queen_Side) {return whiteQueenRook;}
                    }
                    return whiteRook;
                };
                case KNIGHT: return whiteKnight;
                case BISHOP: return whiteBishop;
                case QUEEN: return whiteQueen;
                case KING: return whiteKing;
                default: return 0ULL;
            }
        }
        switch(piece) {
            case PAWN: return blackPawn;
            case ROOK:{
                if (kingSide.has_value()) {
                    if (kingSide == Castle_Side::King_Side) {return blackKingRook;}
                    if (kingSide == Castle_Side::Queen_Side) {return blackQueenRook;}
                }
                return blackRook;
            };
            case KNIGHT: return blackKnight;
            case BISHOP: return blackBishop;
            case QUEEN: return blackQueen;
            case KING: return blackKing;
            default: return 0ULL;
        }

    }

    [[nodiscard]] U64 Get_White_Pieces() const { return whitePieces; }
    [[nodiscard]] U64 Get_Black_Pieces() const { return blackPieces; }
    [[nodiscard]] U64 Get_All_Pieces() const { return whitePieces | blackPieces; }

    [[nodiscard]] Game_State Get_Game_State() const { return boardGameState; }
    [[nodiscard]] int Get_Half_Clock() const { return Half_Clock; }

    [[nodiscard]] U64 Get_Castling_Path(const PieceColour colour, const bool kingSide) const {
        return colour == WHITE ? (kingSide ? whiteKingSideMask : whiteQueenSideMask) :
                                (kingSide ? blackKingSideMask : blackQueenSideMask);

    }
    [[nodiscard]] bool Has_Castling_Rights(const PieceColour colour, const bool kingSide) const {
        return colour == WHITE ?
            (kingSide ? castlingRights.whiteKingSideRookMoves == 0: castlingRights.whiteQueenSideRookMoves == 0) :
            (kingSide ? castlingRights.blackKingSideRookMoves == 0: castlingRights.blackQueenSideRookMoves == 0);
    }

    [[nodiscard]] bool Has_White_King_Side_Castling_Rights() const { return castlingRights.whiteKingSideRookMoves <= 0; }
    [[nodiscard]] bool Has_White_Queen_Side_Castling_Rights() const { return castlingRights.whiteQueenSideRookMoves <= 0; }

    [[nodiscard]] bool Has_Black_King_Side_Castling_Rights() const { return castlingRights.blackKingSideRookMoves <= 0; }
    [[nodiscard]] bool Has_Black_Queen_Side_Castling_Rights() const { return castlingRights.blackQueenSideRookMoves <= 0; }

    void Adjust_Castling_Flags(const Move & move, const bool reset) {
        const int rights = reset == true ? -1 : 1;
        if (move.Get_Castle_Side() == Castle_Side::None && (move.Get_Piece_Type() == ROOK ||
                                                            move.Get_Promotion_Piece() == ROOK)) { return; }
        if (move.Get_Piece_Type() == KING) {
            auto& kingSide = (move.Get_Colour() == WHITE) ?
                                    castlingRights.whiteKingSideRookMoves :
                                    castlingRights.blackKingSideRookMoves;
            auto& queenSide = (move.Get_Colour() == WHITE) ?
                                    castlingRights.whiteQueenSideRookMoves :
                                    castlingRights.blackQueenSideRookMoves;
            kingSide += rights;
            queenSide += rights;
        }
        else {
            auto& rookMoves = (move.Get_Colour() == WHITE) ?
                (move.Get_Castle_Side() == Castle_Side::King_Side ?
                                            castlingRights.whiteKingSideRookMoves :
                                            castlingRights.whiteQueenSideRookMoves) :

                (move.Get_Castle_Side() == Castle_Side::King_Side ?
                                            castlingRights.blackKingSideRookMoves :
                                            castlingRights.blackQueenSideRookMoves);
            rookMoves += rights;
        }
    }

    void Set_Castling_Flag(const PieceColour colour, const bool reset) {
        const int rights = reset == true ? -9999 : 9999;
        switch (colour) {
            case WHITE:
                castlingRights.whiteKingSideRookMoves += rights;
                castlingRights.whiteQueenSideRookMoves += rights;
                break;

            case BLACK:
                castlingRights.blackKingSideRookMoves += rights;
                castlingRights.blackQueenSideRookMoves += rights;
                break;

            default:;
        }
    }

    void Adjust_Half_Clock(const std::optional<bool> increment=std::nullopt, const bool reset=false) {
        if (reset) { Half_Clock = 0; }

        if (increment.has_value()) {
            Half_Clock = increment.value() ? Half_Clock + 1 : Half_Clock - 1;
        }
    }

    void Set_Game_State(const Game_State state) { boardGameState = state; }

    [[nodiscard]] std::string Board_To_Fen() const {
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

        std::string castling;
        if (Has_White_King_Side_Castling_Rights()) castling += 'K';
        if (Has_White_Queen_Side_Castling_Rights()) castling += 'Q';
        if (Has_Black_King_Side_Castling_Rights()) castling += 'k';
        if (Has_Black_Queen_Side_Castling_Rights()) castling += 'q';

        fen += castling.empty() ? "-" : castling;

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
                    whiteKingRook = 1ULL << h1;
                    break;
                case 'Q':
                    castlingRights.whiteQueenSideRookMoves = 0;
                    whiteQueenRook = 1ULL << a1;
                    break;
                case 'k':
                    castlingRights.blackKingSideRookMoves = 0;
                    blackKingRook = 1ULL << h8;
                    break;
                case 'q':
                    castlingRights.blackQueenSideRookMoves = 0;
                    blackQueenRook = 1ULL << a8;
                    break;
            }
        }

        Update_Combined_Bitboards(true);
    }

    static void Print_Move_Details(const Move& move) {
        std::cout << "\nMove Details:\n"
                  << "From: " << Square_To_String(move.Get_From())
                  << " To: " << Square_To_String(move.Get_To())
                  << "\nPiece: " << (move.Get_Colour() == WHITE ? "White " : "Black ") << Get_Piece_Name(move.Get_Piece_Type());

        if(move.Is_Capture()) std::cout << "\nCaptures: " << Get_Piece_Name(move.Get_Captured_Piece());
        if(move.Is_Castling()) std::cout << "\nCastling: " << (move.Get_Castle_Side() == Castle_Side::King_Side ?
                                                                                        "Kingside" : "Queenside");
        if(move.Is_En_Passant()) std::cout << "\nEn Passant Capture";
        if(move.Get_Promotion_Piece() != NO_PIECE) std::cout << "\nPromotion to: " << Get_Piece_Name(move.Get_Promotion_Piece());
        std::cout << "\n";
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
