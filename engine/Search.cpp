#include "Search.h"

#include <algorithm>

#include "../Board/Board_Analyser.h"
#include "../Moves/Move_Generation.h"
#include "../core/Bitboard_Operations.h"
#include "../core/Book.h"
#include "Evaluation.h"
#include "Phases.h"
namespace Engine {
    MoveGeneration MoveGeneration;
    OpeningBook OpeningBook;
    Move bestMove = Move(a8, a8, NO_PIECE, NO_COLOUR);
    static constexpr int INFINITY_SCORE = 1000000;
    static constexpr int MATE_SCORE = 900000;
    static constexpr int MVV_LVA[6][6] = {
        {15, 14, 13, 12, 11, 10},
        {25, 24, 23, 22, 21, 20},
        {35, 34, 33, 32, 31, 30},
        {45, 44, 43, 42, 41, 40},
        {55, 54, 53, 52, 51, 50},
        {0, 0, 0, 0, 0, 0}
    };

    int nodesSearched = 0;
    bool stopSearch = false;

    Move Search::Find_Best_Move(Board& board, const int depth, const PieceColour colour) {
        Move bestMove = Move(a8, a8, NO_PIECE, NO_COLOUR);
        int bestScore = -INFINITY_SCORE;
        std::vector<Move> allMoves = MoveGeneration.Generate_All_Moves(colour, board);
        if (board.moveHistory.size() < 18) {
            if (bool flag = true) {
                BestMoveStat moveStat = OpeningBook.Get_Book_Move(board);
                if (moveStat.bestResults <= 0) {
                    flag = false;
                }
                else {
                    return moveStat.bestMove;
                }
            }
        }

        for (Move& move : allMoves) {
            if (!Board_Analyser::Make_Move(move, true, board)) {
                continue;
            }

            int score = -Minimax(board, depth - 1, -INFINITY_SCORE, -bestScore,
                               colour == WHITE ? BLACK : WHITE);

            board.Undo_Move(false);

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }

        return bestMove;
    }

    void OrderMoves(std::vector<Move>& moves, const PieceColour colour) {
        for (auto& move : moves) {
            int score = 0;

            if (move.Is_Capture()) {
                score = MVV_LVA[move.Get_Captured_Piece()][move.Get_Piece_Type()];
            }


            // score += historyTable[move.Get_From()][move.Get_To()];

            move.score = colour == WHITE ? score : -score;
        }

        std::sort(moves.begin(), moves.end(),
            [](const Move& a, const Move& b) { return a.score > b.score; });
    }

    int Search::Minimax(Board &board, const int depth, int alpha, const int beta, const PieceColour colour) {
        if (depth == 0) {
            const Game_Phase phase = Phase::Phase_Detection(board);
            int score = Evaluator::Evaluate_Complete_Position(board, phase, WHITE) -
                        Evaluator::Evaluate_Complete_Position(board, phase, BLACK);

            return score;
        }

        std::vector<Move> allMoves = MoveGeneration.Generate_All_Moves(colour, board);
        OrderMoves(allMoves, colour);

        if (allMoves.empty()) {
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
                break;
            }
        }
        return bestScore;
    }


};