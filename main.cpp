#include "Board/Board.h"
#include "Game/game.h"
#include "config.h"
#include "core/PGN_Parser.h"
#include "engine/Evaluation.h"


int main() {
    // const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string fen = "rnbqkbnr/pppppppp/8/8/3K4/8/PPPPPPPP/RNBQ1BNR w kq - 0 1";


    Board board;
    board.Initialise_From_Fen(fen);

    Game game = Game(fen);

    // game.Play_Game();

    const std::vector<PGN_Game> games = PGN_Parser::Parse_PGN_File(PGN_FILE_PATH);

    std::cout << games.size() << std::endl;



}
