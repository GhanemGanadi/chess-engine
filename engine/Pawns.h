#pragma once

#include "../core/Bitboard_Operations.h"
class Board;

namespace Engine {

    class Pawn_Evaluation {
        static constexpr std::array<int, 4> SHIELD_PENALTIES = {
            -30,  // Missing directly in front of king
            -25,  // Missing one file away
            -15,  // Missing two files away
            -5    // Missing three files away
        };

        // Additional penalties for open files near king
        static constexpr int OPEN_FILE_PENALTY = -25;
        static constexpr int HALF_OPEN_FILE_PENALTY = -15;

        // Fianchetto position bonuses
        static constexpr int FIANCHETTO_BONUS = 15;
        static constexpr int FIANCHETTO_SHIELD_BONUS = 10;


        public:

        static int Evaluate_Pawn_Chain(const Board& board, PieceColour colour);
        static int Evaluate_Pawn_Structure(const Board& board, PieceColour colour);
        static U64 Get_Shield_Squares(const Board& board, int kingSquare, PieceColour colour);
        static int Evaluate_Pawn_Shield(const Board& board, PieceColour colour);

    };

} // Engine

