#include "comparison.h"
#include "engine/Perft.h"

int main() {
    StockfishTester tester;
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    Board board;
    board.Initialise_From_Fen(fen);
    int depth = 4;
    tester.Test(fen, std::to_string(depth));
    Perft::Perft_Divide(board, depth);
    Perft::Analyse_Differences("../output.txt", "../stockfish.txt");


}