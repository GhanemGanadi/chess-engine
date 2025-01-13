#include "Board/Board.h"
#include "Game/game.h"
#include "config.h"
#include "core/Book.h"
#include "core/PGN_Parser.h"
#include "engine/Evaluation.h"
#include "engine/perft.h"

int main() {

    Board board;
    OpeningBook book = OpeningBook();
    // std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string fen = "r3k1r1/8/8/8/8/8/8/RN2K2R w KQq - 0 1";
    board.Initialise_From_Fen(fen);
    Perft perft;
    perft.Perft_Divide(board, 2);
    Game game = Game(fen);

    // game.Play_Game();

    // std::vector<PGN_Game> games = PGN_Parser::Parse_PGN_File(PGN_FILE_PATH);
    // book.Process_Games(games);
    // book.Save_To_File(OPENING_BOOK_PATH);

}
