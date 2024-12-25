#include "Board/Board.h"
#include "Board/Zobrist.h"
#include "Game/game.h"
#include "config.h"
#include "core/Book.h"
#include "core/PGN_Parser.h"
#include "engine/Evaluation.h"

int main() {

    Zobrist zobrist = Zobrist();
    Board board;
    board.Initialise_From_Fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    std::vector<PGN_Game> games = PGN_Parser::Parse_PGN_File(PGN_FILE_PATH);

    OpeningBook book;
    book.Process_Games(games);
    book.Save_To_File(OPENING_BOOK_PATH);
    auto bestmove = book.Get_Book_Move(board);
    board.Print_Move_Details(bestmove);


    return 0;
}
