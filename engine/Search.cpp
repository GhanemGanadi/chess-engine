#include "Search.h"

#include "Evaluation.h"
#include "Phases.h"
#include "../Board/Board_Analyser.h"
#include "../core/Bitboard_Operations.h"
#include "../Moves/Move_Generation.h"
namespace Engine {
    MoveGeneration MoveGeneration;


    int Search::Minimax(Board &board, int depth, int alpha, int beta, bool isMax) {
        if (depth == 0) {
            const Game_Phase phase = Phase::Phase_Detection(board);
            const int score = Evaluator::Evaluate_Complete_Position(board, phase, WHITE) -
                            Evaluator::Evaluate_Complete_Position(board, phase, BLACK);

            return score;
        }
    }


};