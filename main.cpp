#include "Board/Board.h"
#include "engine/Evaluation.h"
#include "engine/Pawns.h"
#include "Game/game.h"
#include "engine/Phases.h"


int main() {
    // const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string fen = "3p4/4Pp2/5N2/2b1B1R1/pP3b2/1pK3Q1/8/7P w - - 0 1";

    Board board;
    board.Initialise_From_Fen(fen);

    board.Print_Detailed_Board();
    std::cout << Engine::Pawn_Evaluation::Evaluate_Passed_Pawn(board, WHITE);

}
