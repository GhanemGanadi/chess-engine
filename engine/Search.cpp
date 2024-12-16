#include "Search.h"

#include <algorithm>

#include "Evaluation.h"
#include "Phases.h"
#include "../Board/Board_Analyser.h"
#include "../core/Bitboard_Operations.h"
#include "../Moves/Move_Generation.h"
namespace Engine {
    MoveGeneration MoveGeneration;

    Move bestMove = Move(a8, a8, NO_PIECE, NO_COLOUR);
    static constexpr int INFINITY_SCORE = 1000000;
    static constexpr int MATE_SCORE = 900000;
    int nodesSearched = 0;
    bool stopSearch = false;

    Move Search::Find_Best_Move(Board& board, const int depth, const PieceColour colour) {
        Move bestMove = Move(a8, a8, NO_PIECE, NO_COLOUR);
        int bestScore = -INFINITY_SCORE;

        // Generate all possible moves
        std::vector<Move> allMoves = MoveGeneration.Generate_All_Moves(colour, board);

        for (Move& move : allMoves) {
            // Make the move
            if (!Board_Analyser::Make_Move(move, true, board)) {
                continue;
            }

            // Search from opponent's perspective
            int score = -Minimax(board, depth - 1, -INFINITY_SCORE, -bestScore,
                               colour == WHITE ? BLACK : WHITE);

            // Unmake the move
            board.Undo_Move(false);

            // Update best move if better score found
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }

        return bestMove;
    }

    int Search::Minimax(Board &board, const int depth, int alpha, int beta, const PieceColour colour) {
        if (depth == 0) {
            const Game_Phase phase = Phase::Phase_Detection(board);
            int score = Evaluator::Evaluate_Complete_Position(board, phase, WHITE) -
                        Evaluator::Evaluate_Complete_Position(board, phase, BLACK);

            return score;
        }

        std::vector<Move> allMoves = MoveGeneration.Generate_All_Moves(colour, board);

        if (allMoves.size() == 0) {
            return -MATE_SCORE + (MAX_DEPTH - depth);
        }

        int bestScore = -INFINITY_SCORE;

        for (Move& move : allMoves) {

            Board_Analyser::Make_Move(move, true, board);
            int score = -Minimax(board, depth - 1, -beta, -alpha,
                               colour == WHITE ? BLACK : WHITE);

            board.Undo_Move(false);

            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);

            if (alpha >= beta) {
                break;  // Beta cutoff
            }
        }
        return bestScore;
    }


};