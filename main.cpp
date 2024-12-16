#include "Board/Board.h"
#include "engine/Evaluation.h"
#include "engine/Pawns.h"
#include "Game/game.h"
#include "engine/Phases.h"
#include "engine/Search.h"


int main() {
    // const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string fen = "3p4/3kPp2/5n2/2b1B1R1/pP3b2/1pKn2Q1/8/7P w - - 0 1";

    Board board;
    board.Initialise_From_Fen(fen);

    Game game = Game(fen);

    game.Play_Game();

}
