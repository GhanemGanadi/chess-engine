#include "comparison.h"
#include "engine/Perft.h"

int main() {
    StockfishTester tester;
    std::string fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    Board board;
    board.Initialise_From_Fen(fen);
    int depth = 2;
    tester.Test(fen, std::to_string(depth));
    Perft::Perft_Divide(board, depth);
    Perft::Analyse_Differences("../output.txt", "../stockfish.txt");

}