
#include "Attack_Tables.h"
#include "Move_Generator.h"
#include "engine/Game.h"
#include "engine/Perft.h"

int main () {
    std::string fen = "r3k3/p1pp1pb1/bn2pnp1/2qPNr2/1p6/2N2Q1p/PPPBB1PP/R3K2R b KQq - 0 1";
    // std::string fen = "r3k3/p1pp1pb1/bn2pnp1/2qPNr2/1p6/2N2Q2/PPPBB1pP/R3K2R w KQq - 0 1";


    // std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Board board;
    board.Initialise_From_Fen(fen);
    // Game::Play_Game(fen);
    // Board board;
    // board.Initialise_From_Fen(fen);
    // Print_Bitboard(Move_Generator::Get_Pinned_Pieces(e8, BLACK, board));
    // Perft::Perft_Divide_Debugging(fen, 2);
    Perft::Perft_Divide(board, 3);
    Perft::Analyse_Differences("output.txt", "stockfish.txt");
}