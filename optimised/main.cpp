
#include "engine/Game.h"

int main () {
    std::string fen = "r3k3/p1pp1pb1/bn2pnp1/2qPNr2/1p6/2N2Q1p/PPPBB1PP/R3K2R w KQq - 0 1";
    // std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";



    Game::Play_Game(fen);
}