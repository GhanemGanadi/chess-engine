#include "Evaluation.h"

#include <iomanip>
#include "Pawns.h"
#include "Phases.h"

namespace Engine {

    int Evaluator::Get_Piece_Value(const PieceType piece, const Game_Phase phase) {
        switch (phase) {
            case Game_Phase::OPENING:
                return PieceValues::OPENING_VALUES[piece];

            case Game_Phase::MIDDLE_GAME:
                return PieceValues::MIDDLE_GAME_VALUES[piece];

            case Game_Phase::END_GAME:
                return PieceValues::END_GAME_VALUES[piece];

            default:
                return PieceValues::MIDDLE_GAME_VALUES[piece];
        }
    }


    int Evaluator::Evaluate_Material(const Board &board, const PieceColour colour, const Game_Phase phase) {
        int score = 0;
        for(const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN}) {
            const U64 pieceBB = board.Get_Piece_Bitboard(piece, colour);
            score += Count_Bits(pieceBB) * Get_Piece_Value(piece, phase);

            U64 positionBB = pieceBB;

            while (positionBB) {
                int square = Get_LS1B_Index(positionBB);
                if (colour == BLACK) { square = FLIP_SQUARE(square); }
                positionBB &= positionBB - 1;

                switch (piece) {
                    case PAWN:
                        score += PAWN_TABLE[square];
                    break;

                    case KNIGHT:
                        score += KNIGHT_TABLE[square];
                    break;

                    case BISHOP:
                        score += BISHOP_TABLE[square];
                    break;

                    case ROOK:
                        score += ROOK_TABLE[square];
                    break;

                    case QUEEN:
                        score += QUEEN_TABLE[square];
                    break;

                    default: break;
                }
            }
        }
        return score;
    }


    int Evaluator::Evaluate_Piece_Mobility(Board &board, const PieceColour colour) {
        int score = 0;

        for (const PieceType piece : {KNIGHT, ROOK, QUEEN}) {

            U64 pieceBB = board.Get_Piece_Bitboard(piece, colour);

            while (pieceBB) {
                const int pieceSquare = Get_LS1B_Index(pieceBB);
                pieceBB &= pieceBB - 1;

                const U64 legalMoves = moveGen.Get_Legal_Moves(pieceSquare, colour, piece, board);
                const int moveCount = Count_Bits(legalMoves);

                score += moveCount * MOBILITY_WEIGHTS[piece];
            }
        }
        return score;
    }
    // Debug versions of component evaluation functions
    int Evaluator::Debug_Evaluate_Material(const Board& board, const PieceColour colour, const Game_Phase phase) {
        int score = 0;
        std::cout << "Material Evaluation for " << (colour == WHITE ? "White" : "Black") << ":\n";

        for(const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN}) {
            const U64 pieceBB = board.Get_Piece_Bitboard(piece, colour);
            int pieceCount = Count_Bits(pieceBB);
            int materialScore = pieceCount * Get_Piece_Value(piece, phase);
            score += materialScore;

            std::cout << "  " << Get_Piece_Name(piece) << "s: " << pieceCount
                     << " × " << Get_Piece_Value(piece, phase) << " = " << materialScore << "\n";

            // Piece-square table evaluation
            int positionScore = 0;
            U64 positionBB = pieceBB;
            while (positionBB) {
                int square = Get_LS1B_Index(positionBB);
                if (colour == BLACK){ square = FLIP_SQUARE(square); }
                int squareScore = 0;

                switch (piece) {
                    case PAWN:
                        squareScore = PAWN_TABLE[square];
                    break;

                    case KNIGHT:
                        squareScore = KNIGHT_TABLE[square];
                    break;

                    case BISHOP:
                        squareScore = BISHOP_TABLE[square];
                    break;

                    case ROOK:
                        squareScore = ROOK_TABLE[square];
                    break;

                    case QUEEN:
                        squareScore = QUEEN_TABLE[square];
                    break;

                    case KING:
                        // Since king has different tables for middlegame and endgame, you might want to check game phase
                            squareScore = KING_MIDDLE_GAME_TABLE[square];
                    break;

                    default:
                        squareScore = 0;
                    break;
                }

                positionScore += squareScore;
                positionBB &= positionBB - 1;

                std::cout << "    Position bonus at " << Square_To_String(static_cast<Squares>(square))
                          << ": " << squareScore << "\n";
            }

            score += positionScore;
            std::cout << "  Total position bonus for " << Get_Piece_Name(piece) << "s: " << positionScore << "\n";
        }

        std::cout << "Total Material and Position Score: " << score << "\n";
        return score;
    }


    // NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE NOT DONE
    int Evaluator::Evaluate_Knight_Position(const Board& board, const PieceColour colour) {
        int score = 0;
        U64 knights = board.Get_Piece_Bitboard(KNIGHT, colour);
        while (knights) {
            const int singleKnightSquare = Get_LS1B_Index(knights);
            knights &= knights - 1;

            if (Is_Knight_Trapped(board, singleKnightSquare, colour)) {
                score -= 150;
            }
        }

        return score;
    }

    int Evaluator::Evaluate_Bishop_Pair(const Board &board, const PieceColour colour) {
        return Count_Bits(board.Get_Piece_Bitboard(BISHOP, colour)) >= 2 ? BISHOP_PAIR_BONUS : 0;
    }


    bool Evaluator::Is_Knight_Trapped(const Board& board, const int square, const PieceColour colour) {

        if (colour == WHITE) {
            if (square == a7) {
                U64 trappingPawns = board.Get_Piece_Bitboard(PAWN, BLACK);
                if ((trappingPawns & (1ULL << b7)) &&
                    (trappingPawns & (1ULL << a6))) {
                    return true;
                    }
            }
            else if (square == h7) {
                U64 trappingPawns = board.Get_Piece_Bitboard(PAWN, BLACK);
                if ((trappingPawns & (1ULL << g7)) &&
                    (trappingPawns & (1ULL << h6))) {
                    return true;
                    }
            }
        }
        // Black knight traps
        else {
            if (square == a2) {
                U64 trappingPawns = board.Get_Piece_Bitboard(PAWN, WHITE);
                if ((trappingPawns & (1ULL << b2)) &&
                    (trappingPawns & (1ULL << a3))) {
                    return true;
                    }
            }
            else if (square == h2) {
                U64 trappingPawns = board.Get_Piece_Bitboard(PAWN, WHITE);
                if ((trappingPawns & (1ULL << g2)) && // pawn on g2
                    (trappingPawns & (1ULL << h3))) { // pawn on h3
                    return true;
                    }
            }
        }

        return false;
    }


    int Evaluator::Square_Control_Value(const int square, const Game_Phase phase, Board& board) {
        int score = 0;
        const U64 target_square = 1ULL << square;

        U64 whitePieces = board.Get_White_Pieces();

        while(whitePieces) {
            const int piece_square = Get_LS1B_Index(whitePieces);
            const PieceType piece = Board_Analyser::Find_Piece_Type(piece_square, WHITE, board);

            const U64 control = moveGen.Get_Legal_Moves(piece_square, WHITE, piece, board);

            if(control & target_square) {
                score += Get_Piece_Value(piece, phase);
            }

            whitePieces &= whitePieces - 1;
        }

        U64 blackPieces = board.Get_Black_Pieces();

        while(blackPieces) {
            const int piece_square = Get_LS1B_Index(blackPieces);
            const PieceType piece = Board_Analyser::Find_Piece_Type(piece_square, BLACK, board);

            const U64 control = moveGen.Get_Legal_Moves(piece_square, BLACK, piece, board);

            if(control & target_square) {
                score -= Get_Piece_Value(piece, phase);
            }

            blackPieces &= blackPieces - 1;
        }
        return score;
    }

    void Evaluator::Debug_Square_Control(Board& board, const Game_Phase phase) {
        std::cout << "\nSquare Control Values (positive = White control, negative = Black control)\n";
        std::cout << "Values shown in pawns (100 centipawns = 1.0)\n\n";

        // Print file labels
        std::cout << "     a     b     c     d     e     f     g     h\n";
        std::cout << "  -----------------------------------------------\n";

        // Print each rank
        for(int rank = 0; rank < 8; rank++) {
            // Print rank number
            std::cout << 8 - rank << " |";

            // Print square values for this rank
            for(int file = 0; file < 8; file++) {
                const int square = rank * 8 + file;
                const float control = Square_Control_Value(square, phase, board) / 100.0f;

                // Format with fixed precision
                std::cout << std::fixed << std::setprecision(1);

                // Format the output to align properly
                std::cout << std::setw(5) << control << "|";
            }

            // Print rank number again on the right side
            std::cout << " " << 8 - rank << "\n";
            std::cout << "  -----------------------------------------------\n";
        }

        // Print file labels again at the bottom
        std::cout << "     a     b     c     d     e     f     g     h\n\n";
    }

    int Evaluator::Evaluate_King_Safety(const Board &board, const PieceColour colour) {
        int score = Pawn_Evaluation::Evaluate_Pawn_Shield(board, colour) + Pawn_Evaluation::Evaluate_Open_File(board, colour);
        U64 kingBB = board.Get_Piece_Bitboard(KING, colour);

        return score;
    }

    bool Evaluator::Is_Piece_Hanging(const Board &board, const int square, const PieceColour colour) {
        return Board_Analyser::Is_Square_Attacked(square, colour, board) == -1;
    }


    int Evaluator::Evaluate_Complete_Position(Board &board, const Game_Phase phase, const PieceColour colour) {
        int score = 0;

        score += Evaluate_Material(board, colour, phase);

        score += Evaluate_Piece_Mobility(board, colour);
        score += Evaluate_King_Safety(board, colour);
        score += Evaluate_Bishop_Pair(board, colour);
        score += Pawn_Evaluation::Complete_Pawn_Evaluation(board, colour);


        return score;
    }




}
