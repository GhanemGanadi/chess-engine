
#include "Phases.h"

#include "../Board/Board.h"


namespace Engine {

    Game_Phase Phase::Phase_Detection(const Board &board) {
        int phase = 0;
        for (const PieceType piece : {KNIGHT, BISHOP, ROOK, QUEEN}) {
            const U64 combinedPieces = board.Get_Piece_Bitboard(piece, WHITE) |
                                        board.Get_Piece_Bitboard(piece, BLACK);

            phase += Count_Bits(combinedPieces) * PIECE_PHASE_WEIGHTS[piece];
        }
        phase = std::min(phase, PHASE_SCALE);

        const bool queensExist = Count_Bits(board.Get_Piece_Bitboard(QUEEN, WHITE) |
                                        board.Get_Piece_Bitboard(QUEEN, BLACK));

        if (!queensExist && phase > 64) {phase = std::min(phase, 64); }

        bool pawnsAndKings = true;
        for (const PieceType piece : {KNIGHT, BISHOP, ROOK, QUEEN}) {
            if (board.Get_Piece_Bitboard(piece, WHITE) |
                board.Get_Piece_Bitboard(piece, BLACK)) {
                pawnsAndKings = false;
                break;
                }
        }
        if (pawnsAndKings) { return Game_Phase::END_GAME; }

        if (phase > 192){ return Game_Phase::OPENING; }
        if (phase >= 64){ return Game_Phase::MIDDLE_GAME; }

        return Game_Phase::END_GAME;
    }

}
