#include "Search.h"

#include "Evaluation.h"
#include "../Board/Board_Analyser.h"
#include "../core/Bitboard_Operations.h"
#include "../Moves/Move_Generation.h"
namespace Engine {
    MoveGeneration Move_Generation;
    Board tempBoard;
    Board originalBoard;

    // int Search::Minimax(const int depth, int alpha, int beta, const bool isMax) {
    //     if(depth == 0) {
    //         return Evaluator::Evaluate_Material(tempBoard, WHITE) -
    //                Evaluator::Evaluate_Material(tempBoard, BLACK);
    //     }
    //     const PieceColour colour = isMax ? WHITE : BLACK;
    //     std::vector<Move> moves = Move_Generation.Generate_All_Moves(colour, tempBoard);
    //
    //     for(Move singleMove : moves) {
    //         Board_Analyser::Make_Move(singleMove, tempBoard);
    //         int score = Minimax(depth -1, alpha, beta, !isMax);
    //         tempBoard = originalBoard;
    //
    //         if(isMax) {
    //             alpha = std::max(alpha, score);
    //             if(beta <= alpha) {break;}
    //         }
    //
    //         else {
    //             beta = std::min(beta, score);
    //             if(beta <= alpha) {break;}
    //         }
    //     }
    //     return isMax ? alpha : beta;
    // }
    //
    // Move Search::Find_Best_Move(const int depth, const PieceColour colour) {
    //     Move bestMove = Move(a1, a1, NO_PIECE, NO_COLOUR);
    //     int bestValue = -999999;
    //     const bool isMax = colour == WHITE;
    //
    //     std::vector<Move> moves = Move_Generation.Generate_All_Moves(colour, tempBoard);
    //
    //     for(Move singleMove : moves) {
    //         Board_Analyser::Make_Move(singleMove, tempBoard);
    //         int value = Minimax(depth - 1, -99999, 99999, isMax);
    //         tempBoard = originalBoard;
    //
    //         if(value > bestValue) {
    //             bestValue = value;
    //             bestMove = singleMove;
    //         }
    //     }
    //     return bestMove;
    // }


    void Search::Set_Board(const Board& board) {
        tempBoard = board;
        originalBoard = board;
    }

};