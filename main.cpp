#include "Board/Board.h"
#include "Game/game.h"
#include "core/PGN_Parser.h"
#include "engine/Evaluation.h"
#include "engine/King.h"
#include "engine/Pawns.h"
#include "engine/Phases.h"
#include "engine/Search.h"


int main() {
    // const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string fen = "rnbqkbnr/pppppppp/8/8/3K4/8/PPPPPPPP/RNBQ1BNR w kq - 0 1";


    Board board;
    board.Initialise_From_Fen(fen);

    Game game = Game(fen);

    // game.Play_Game();

    std::vector<PGN_Game> games = PGN_Parser::Parse_PGN_File("/Users/ghanem/CLionProjects/chess-engine/core/magnus_pgn.txt");

    std::cout << games.size() << std::endl;



}
