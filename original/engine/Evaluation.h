#pragma once

#include "../core/Bitboard_Operations.h"
#include "../Moves/Move_Generation.h"

enum class Game_Phase;
class Board;

struct PieceValues {
    static constexpr std::array<int, 6> OPENING_VALUES = {100, 325, 325, 500, 900, 0};  // P, N, B, R, Q, K
    static constexpr std::array<int, 6> MIDDLE_GAME_VALUES = {100, 320, 330, 500, 900, 0};
    static constexpr std::array<int, 6> END_GAME_VALUES = {120, 305, 345, 525, 880, 0};
};

static constexpr int FLIP_SQUARE(const int square) { return (square ^ 56); }

namespace Engine {
    inline MoveGeneration moveGen;

    class Evaluator {

    static constexpr std::array<int, 64> PAWN_TABLE = {
        0,  0,  0,  0,  0,  0,  0,  0,  // a8-h8
       50, 50, 50, 50, 50, 50, 50, 50,  // a7-h7
       10, 10, 20, 30, 30, 20, 10, 10,  // a6-h6
        5,  5, 10, 25, 25, 10,  5,  5,  // a5-h5
        0,  0,  0, 20, 20,  0,  0,  0,  // a4-h4
        5, -5,-10,  0,  0,-10, -5,  5,  // a3-h3
        5, 10, 10,-20,-20, 10, 10,  5,  // a2-h2
        0,  0,  0,  0,  0,  0,  0,  0   // a1-h1
   };

    static constexpr std::array<int, 64> KNIGHT_TABLE = {
        -50,-40,-30,-30,-30,-30,-40,-50,  // a8-h8
        -40,-20,  0,  0,  0,  0,-20,-40,  // a7-h7
        -30,  0, 10, 15, 15, 10,  0,-30,  // a6-h6
        -30,  5, 15, 20, 20, 15,  5,-30,  // a5-h5
        -30,  0, 15, 20, 20, 15,  0,-30,  // a4-h4
        -30,  5, 10, 15, 15, 10,  5,-30,  // a3-h3
        -40,-20,  0,  5,  5,  0,-20,-40,  // a2-h2
        -50,-40,-30,-30,-30,-30,-40,-50   // a1-h1
    };

    static constexpr std::array<int, 64> BISHOP_TABLE = {
        -20,-10,-10,-10,-10,-10,-10,-20,  // a8-h8
        -10,  0,  0,  0,  0,  0,  0,-10,  // a7-h7
        -10,  0,  5, 10, 10,  5,  0,-10,  // a6-h6
        -10,  5,  5, 10, 10,  5,  5,-10,  // a5-h5
        -10,  0, 10, 10, 10, 10,  0,-10,  // a4-h4
        -10, 10, 10, 10, 10, 10, 10,-10,  // a3-h3
        -10,  5,  0,  0,  0,  0,  5,-10,  // a2-h2
        -20,-10,-10,-10,-10,-10,-10,-20   // a1-h1
    };

    static constexpr std::array<int, 64> ROOK_TABLE = {
         0,  0,  0,  0,  0,  0,  0,  0,  // a8-h8
         5, 10, 10, 10, 10, 10, 10,  5,  // a7-h7
        -5,  0,  0,  0,  0,  0,  0, -5,  // a6-h6
        -5,  0,  0,  0,  0,  0,  0, -5,  // a5-h5
        -5,  0,  0,  0,  0,  0,  0, -5,  // a4-h4
        -5,  0,  0,  0,  0,  0,  0, -5,  // a3-h3
        -5,  0,  0,  0,  0,  0,  0, -5,  // a2-h2
         0,  0,  0,  5,  5,  0,  0,  0   // a1-h1
    };

    static constexpr std::array<int, 64> QUEEN_TABLE = {
        -20,-10,-10, -5, -5,-10,-10,-20,  // a8-h8
        -10,  0,  0,  0,  0,  0,  0,-10,  // a7-h7
        -10,  0,  5,  5,  5,  5,  0,-10,  // a6-h6
         -5,  0,  5,  5,  5,  5,  0, -5,  // a5-h5
          0,  0,  5,  5,  5,  5,  0, -5,  // a4-h4
        -10,  5,  5,  5,  5,  5,  0,-10,  // a3-h3
        -10,  0,  5,  0,  0,  0,  0,-10,  // a2-h2
        -20,-10,-10, -5, -5,-10,-10,-20   // a1-h1
    };

   static constexpr std::array<int, 64> KING_MIDDLE_GAME_TABLE = {
       -30,-40,-40,-50,-50,-40,-40,-30,  // a8-h8
       -30,-40,-40,-50,-50,-40,-40,-30,  // a7-h7
       -30,-40,-40,-50,-50,-40,-40,-30,  // a6-h6
       -30,-40,-40,-50,-50,-40,-40,-30,  // a5-h5
       -20,-30,-30,-40,-40,-30,-30,-20,  // a4-h4
       -10,-20,-20,-20,-20,-20,-20,-10,  // a3-h3
        20, 20,  0,  0,  0,  0, 20, 20,  // a2-h2
        20, 30, 10,  0,  0, 10, 30, 20   // a1-h1
   };

   static constexpr std::array<int, 64> KING_ENDGAME_TABLE = {
       -50,-40,-30,-20,-20,-30,-40,-50,  // a8-h8
       -30,-20,-10,  0,  0,-10,-20,-30,  // a7-h7
       -30,-10, 20, 30, 30, 20,-10,-30,  // a6-h6
       -30,-10, 30, 40, 40, 30,-10,-30,  // a5-h5
       -30,-10, 30, 40, 40, 30,-10,-30,  // a4-h4
       -30,-10, 20, 30, 30, 20,-10,-30,  // a3-h3
       -30,-30,  0,  0,  0,  0,-30,-30,  // a2-h2
       -50,-30,-30,-30,-30,-30,-30,-50   // a1-h1
   };

    static constexpr std::array<int, 6> MOBILITY_WEIGHTS = {0, 4, 5, 4, 3, 0};
    static constexpr int BISHOP_PAIR_BONUS = 30;


    public:
        static int Evaluate_Material(const Board& board, PieceColour colour, Game_Phase phase);
        static int Evaluate_Piece_Mobility(Board& board, PieceColour colour);

        static int Evaluate_Knight_Position(const Board& board, PieceColour colour);
        static int Evaluate_Bishop_Pair(const Board& board, PieceColour colour);
        static bool Is_Knight_Trapped(const Board& board, int square, PieceColour colour);

        static bool Is_Piece_Hanging(const Board& board, int square, PieceColour colour);

        static int Evaluate_Complete_Position(Board &board, Game_Phase phase, PieceColour colour);
        static int Get_Piece_Value(PieceType piece, Game_Phase phase);
        static int Evaluate_Piece_Safety(const Board& board, PieceColour colour);
        static int Square_Control_Value(int square, Game_Phase phase, Board& board);

        static int Debug_Evaluate_Position(Board& board, PieceColour colour, Game_Phase phase);
        static int Debug_Evaluate_Material(const Board& board, PieceColour colour, Game_Phase phase);
        static void Debug_Square_Control(Board& board, Game_Phase phase);
    };
}
