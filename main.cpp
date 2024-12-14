#include "Board/Board.h"
#include "engine/Evaluation.h"
#include "engine/Pawns.h"
#include "Game/game.h"
#include "engine/Phases.h"


int main() {
    // std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string fen = "3p4/4Pp2/5N2/2b1B1R1/pP3b2/1pK3Q1/8/8 w - - 0 1";

    Board board;
    board.Initialise_From_Fen(fen);

}
