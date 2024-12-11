#include "Board/Board.h"
#include "engine/Evaluation.h"
#include "engine/Pawns.h"
#include "Game/game.h"
#include "engine/Phases.h"


int main() {
    // std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR w KQkq - 0 1";

    Board board;
    board.Initialise_From_Fen(fen);

    // std::cout << Engine::Evaluator::Debug_Evaluate_Position(board, WHITE, Game_Phase::MIDDLE_GAME);

    // board.Print_Detailed_Board();
    // Engine::Evaluator::Debug_Square_Control(board, Game_Phase::MIDDLE_GAME);


    U64 pawn = 1ULL << Get_LS1B_Index(board.Get_White_Pieces());
    // board.Print_Bitboard(pawn);
    // board.Print_Bitboard(pawn << 7 | pawn >> 9);
    std::cout << Engine::Pawn_Evaluation::Evaluate_Pawn_Chain(board, WHITE);



}
