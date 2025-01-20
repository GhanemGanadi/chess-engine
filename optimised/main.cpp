#include "comparison.h"

int main () {
    Board board;

    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4K1R w KQkq - 0 1";
    // std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    board.Initialise_From_Fen(fen);
    const BB bb = Move_Generator::Get_Legal_Moves(e2, BISHOP, WHITE, board);

    Print_Bitboard(bb);




}