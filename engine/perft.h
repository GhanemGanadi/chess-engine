#pragma once
#include "../Board/Board.h"
#include "../Moves/Move_Generation.h"
#include "../Board/Board_Analyser.h"

struct PerftStats {
    U64 nodes = 0;
    U64 captures = 0;
    U64 enPassant = 0;
    U64 castles = 0;
    U64 promotions = 0;
    U64 checks = 0;
    U64 checkmates = 0;

    void Add(const PerftStats& other) {
        nodes += other.nodes;
        captures += other.captures;
        enPassant += other.enPassant;
        castles += other.castles;
        promotions += other.promotions;
        checks += other.checks;
        checkmates += other.checkmates;
    }

    void Print() const {
        std::cout << "Nodes: " << nodes << "\n"
                  << "Captures: " << captures << "\n"
                  << "En Passant: " << enPassant << "\n"
                  << "Castles: " << castles << "\n"
                  << "Promotions: " << promotions << "\n"
                  << "Checks: " << checks << "\n"
                  << "Checkmates: " << checkmates << "\n";
    }
};

class Perft {
public:

    // Basic node counter
    int Run_Perft(Board& board, int depth);

    // Detailed move counting
    void Perft_Divide(Board& board, int depth);
    void Perft_Divide_Debugging(Board& board, int depth);

    // Full statistics
    PerftStats Run_Perft_Stats(Board& board, int depth, PieceColour side);

private:
    static bool Is_Move_Legal(Move& move, Board& board);
    static void Make_Move_And_Count(Move& move, Board& board, PerftStats& stats);
};