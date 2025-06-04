#include "comparison.h"
#include "engine/Perft.h"

int main() {
    StockfishTester tester;
    std::string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    Board board;
    board.Initialise_From_Fen(fen);
    int depth = 3;
    tester.Stockfish_Perft(fen, std::to_string(depth));
    Perft::Perft_Divide(board, depth);
    Perft::Analyse_Differences("../output.txt", "../stockfish.txt");



}