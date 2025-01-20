#pragma once

class Board;

enum class Game_Phase {
    OPENING,
    MIDDLE_GAME,
    END_GAME
};


namespace Engine {
    class Phase {

        static constexpr int PHASE_SCALE = 256;

        // Lookup table for (PHASE_SCALE * weight) / TOTAL_PHASE
        static constexpr int PIECE_PHASE_WEIGHTS[] = {
            0,      // PAWN
            11,     // KNIGHT  (256/24 ≈ 10.67)
            11,     // BISHOP
            22,     // ROOK    (2 * 256/24 ≈ 21.33)
            43      // QUEEN   (4 * 256/24 ≈ 42.67)
        };
        public:
        static Game_Phase Phase_Detection(const Board& board) ;


    };
}
