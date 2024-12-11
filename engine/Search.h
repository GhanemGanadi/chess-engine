#pragma once
#include <vector>
#include "../Board/Board.h"
class Board;
struct Move;

namespace Engine {
    class Search {
        static constexpr int MAX_DEPTH = 4;
        public:
        static int Minimax(int depth, int alpha, int beta, bool isMax);
        static Move Find_Best_Move(int depth, PieceColour colour);
        static int Quiescence_Search(Board& board, int alpha, int beta);
        static void Set_Board(const Board& board);

    };
}
