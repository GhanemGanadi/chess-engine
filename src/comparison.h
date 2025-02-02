#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include "Move_Generator.h"

class Performance_Test {

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::nanoseconds;

    Board board;

    struct TestResults {
        std::string test_name;
        double time_ms;
        int nodes;
        double nodes_per_second;
    };
    std::vector<TestResults> results;

    const std::vector<std::string> test_positions = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
    };

public:

    void Piece_Type_Test() {
        int count = 1;
        for (const auto& fen : test_positions) {
            std::cout << "----------- FEN " << count << " ----------" << std::endl;
            board.Initialise_From_Fen(fen);

            for (const PieceType piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
                std::string piece_name;

                switch (piece) {
                    case PAWN: piece_name = "PAWN"; break;
                    case KNIGHT: piece_name = "KNIGHT"; break;
                    case BISHOP: piece_name = "BISHOP"; break;
                    case ROOK: piece_name = "ROOK"; break;
                    case QUEEN: piece_name = "QUEEN"; break;
                    case KING: piece_name = "KING"; break;
                    default: piece_name = "UNKNOWN"; break;
                }

                auto start = Clock::now();
                BB moves = 0ULL;
                for (int square = 0; square < 64; ++square) {
                    moves |= Move_Generator::Get_Legal_Moves(square, piece, WHITE, board);
                }
                auto end = Clock::now() - start;

                double timeTaken = std::chrono::duration<double, std::milli>(end).count();
                TestResults test_results = {
                        piece_name,
                        timeTaken,
                        Count_Bits(moves),
                        static_cast<double>(Count_Bits(moves)) / (timeTaken / 1000.0)
                        };
                results.push_back(test_results);
                std::cout << piece_name << " moves:\n";
                std::cout << "New implementation: " << timeTaken << "ms\n";

            }
            ++count;
        }

    }
};
