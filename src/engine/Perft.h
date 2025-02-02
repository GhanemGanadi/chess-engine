#pragma once
#include "../Move_Generator.h"

struct PerftStats {
    BB nodes = 0;
    BB captures = 0;
    BB enPassant = 0;
    BB castles = 0;
    BB promotions = 0;
    BB checks = 0;
    BB checkmates = 0;

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

    static int Run_Perft(Board& board, int depth);
    static void Perft_Divide(Board& board, int depth);
    static void Perft_Divide_Debugging(const std::string& fen, int depth);
    static void Analyse_Differences(const std::string& my_output_file, const std::string& stockfish_file);

};