
#include "King.h"

namespace Engine{

    U64 King_Evaluation::Get_King_Attack_Zone(const Board &board, PieceColour colour) {
        const int kingSquare = Get_Square(board.Get_Piece_Bitboard(KING, colour));
        U64 kingMoves = 0ULL;

        for (const int i : {-1,1,-8,8, 9,-9,7,-7}) {
            const int kingDirection = kingSquare + i;
            kingMoves |= MoveGeneration::Get_King_Moves(kingDirection);
        }
        return kingMoves;
    }

    int King_Evaluation::Evaluate_King_Attack_Zone(const Board &board, const PieceColour colour) {
        const U64 kingZone = Get_King_Attack_Zone(board, colour);
        const PieceColour enemyColour = colour == WHITE ? BLACK : WHITE;
        int score = 0;
        int numberOfAttackers = 0;

        for (const PieceType piece : {KNIGHT, BISHOP, ROOK, QUEEN}) {

            U64 pieceBB = board.Get_Piece_Bitboard(piece, enemyColour);

            while (pieceBB) {
                const int singlePiece = Get_LS1B_Index(pieceBB);
                pieceBB &= pieceBB - 1;
                U64 pieceInKingZone = kingZone & MoveGeneration::Get_Piece_Attacks(piece, singlePiece, enemyColour, board);
                if (pieceInKingZone) {
                    score += (Count_Bits(pieceInKingZone) * ATTACK_WEIGHTS[piece]);
                    numberOfAttackers++;
                }
            }

        }
        score = score * (NO_OF_ATTACKERS_MULTIPLIER[numberOfAttackers]) / 100;
        return -score;
    }

    int King_Evaluation::Evaluate_Castling_Status(const Board &board, const PieceColour colour) {
        if (board.moveHistory.size() < 16) {
            return 0;
        }

        if (colour == WHITE) {
            if (board.castlingRights.whiteKingSideRookMoves >= 9999 ||
                board.castlingRights.whiteQueenSideRookMoves >= 9999) {
                return CASTLED_BONUS;
            }
        }
        if (colour == BLACK) {
            if (board.castlingRights.blackKingSideRookMoves >= 9999 ||
                board.castlingRights.blackQueenSideRookMoves >= 9999) {
                return CASTLED_BONUS;
            }
        }
        return UNCASTLED_BONUS * ((board.moveHistory.size() - 16)/2);
    }

    int King_Evaluation::Complete_King_Evaluation(const Board &board, const PieceColour colour) {
        int score = 0;
        score += Evaluate_Castling_Status(board, colour);
        score += Evaluate_King_Attack_Zone(board, colour);

        return score;
    }


}