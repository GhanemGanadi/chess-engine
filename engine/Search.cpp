#include "Search.h"
#include <algorithm>
#include <fstream>

#include "../Board/Board_Analyser.h"
#include "../Moves/Move_Generation.h"
#include "../core/Bitboard_Operations.h"
#include "../core/Book.h"
#include "Evaluation.h"
#include "Phases.h"

// std::ofstream file("../core/output.txt");
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
        bool flag = true;
        if (board.moveHistory.size() < 18) {
            if (flag) {
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
            Board_Analyser::Make_Move(move, true, board);
            // file << " DEPTH: " << depth << std::endl;
            // file << Square_To_String(move.Get_To()) << " " << Square_To_String(move.Get_From()) << " "
            // << Get_Piece_Name(move.Get_Piece_Type()) << std::endl;
            const int score = Minimax(board, depth -  1, -INFINITY_SCORE, INFINITY_SCORE,
                               board.currentTurn);

            board.Undo_Move(false);

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }

        // file.close();
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

    int Search::Minimax(Board &board, const int depth, int alpha, int beta, const PieceColour colour) {
        // file << " DEPTH: " << depth << std::endl;
        if (depth == 0) {
            const Game_Phase phase = Phase::Phase_Detection(board);
            const int score = Evaluator::Evaluate_Complete_Position(board, phase, WHITE) -
                        Evaluator::Evaluate_Complete_Position(board, phase, BLACK);

            // file << " Score: " << score << std::endl;
            return score;
        }

        std::vector<Move> allMoves = MoveGeneration.Generate_All_Moves(colour, board);
        // OrderMoves(allMoves, colour);

        if (allMoves.empty()) {
            const int score = MATE_SCORE * (MAX_DEPTH - depth);
            return colour == WHITE ? score : -score;
        }

        int bestScore = colour == WHITE ? -INFINITY_SCORE : INFINITY_SCORE;

        for (Move& move : allMoves) {
            // file << Square_To_String(move.Get_To()) << " " << Square_To_String(move.Get_From()) << " "
                // << Get_Piece_Name(move.Get_Piece_Type()) << std::endl;
            Board_Analyser::Make_Move(move, true, board);
            int score = Minimax(board, depth - 1, alpha, beta, board.currentTurn);

            board.Undo_Move(false);

            bestScore = colour == WHITE ? std::max(bestScore, score) : std::min(bestScore, score);

            if (colour == WHITE) { alpha = std::max(alpha, score); }
            else { beta = std::min(beta, score); }

            if (beta <= alpha) {
                // break;
                // file << "PRUNED" << std::endl;
            }
        }
        return bestScore;
    }


};