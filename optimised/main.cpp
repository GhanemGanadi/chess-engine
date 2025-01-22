#include "comparison.h"

int main () {
    Board board;

    std::string fen = "r3k3/p1pp1pb1/bn2pnp1/2qPNr2/1p6/2N2Q1p/PPPBB1PP/R3K2R w KQq - 0 1";
    // std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    board.Initialise_From_Fen(fen);
    Squares bit = e1;
    PieceType piece = KING;
    const BB bb = Move_Generator::Get_Legal_Moves(bit, piece, WHITE, board);
    board.Print_Detailed_Board();
    Print_Bitboard(bb);




}