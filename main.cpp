#include <fstream>\
#include "Board/Board.h"
#include "Board/Zobrist.h"
#include "Game/game.h"
#include "config.h"
#include "core/Book.h"
#include "core/PGN_Parser.h"
#include "engine/Evaluation.h"

int main() {

    Board board;
    OpeningBook book = OpeningBook();
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // std::string fen = "r1bk2n1/p1p1np2/Pp6/1Bbp2p1/3Pp3/1PP1PP2/6Pr/RNBK3R w - - 0 1";
    // board.Initialise_From_Fen(fen2);
    Game game = Game(fen);

    game.Play_Game();
    //
    //
    // std::vector<PGN_Game> games = PGN_Parser::Parse_PGN_File(PGN_FILE_PATH);
    // book.Process_Games(games);
    // book.Save_To_File(OPENING_BOOK_PATH);

}
