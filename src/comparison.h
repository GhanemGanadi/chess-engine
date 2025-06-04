#pragma once

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include "Move_Generator.h"
#include "engine/Game.h"

class StockfishTester {
    FILE* pipe;

public:

    StockfishTester() {
        pipe = popen("/Users/ghanem/CLionProjects/chess-engine/stockfish", "r+");

        if (!pipe) {
            throw std::runtime_error("Failed to open stockfish");
        }
    }
    ~StockfishTester() {
        pclose(pipe);
    }

    void Send_Command(const std::string& cmd) {
        fprintf(pipe, "%s\n", cmd.c_str());
        fflush(pipe);
    }
    std::string Get_Response() {
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            std::string line(buffer);
            if (line.find("info") != std::string::npos ||
                line.find("Stockfish") != std::string::npos ||
                line.find("NNUE") != std::string::npos) {
                continue;
    }
            if (line.find(":") != std::string::npos ||
                line.find("Nodes searched") != std::string::npos) {
                return line;
            }
        }
        return "";
    }

    void Stockfish_Perft(const std::string &fen, const std::string& depth) {
        Board board;
        board.Initialise_From_Fen(fen);
        Send_Command("position fen " + fen);
        Send_Command("go perft " + depth);
        std::vector<Move> stockfish_moves;
        std::string line;
        std::ofstream fout("../stockfish.txt");
        do {
            line = Get_Response();
            fout << line;
            Move temp_move = Game::Parse_Move(line.substr(0,4), board.current_turn, board);
            if (temp_move != Move()) { stockfish_moves.push_back(temp_move); }

        } while (line.find("Nodes searched") == std::string::npos);
        fout.close();
        // Compare_Moves(fen, stockfish_moves);

    }

    void Compare_Moves(const std::string& fen, const std::vector<Move>& stockfish_moves) {
        Board board;
        board.Initialise_From_Fen(fen);
        const std::vector<Move> my_moves = Move_Generator::Generate_All_Moves(board);

        std::map<std::string, bool> stockfish_move_map;
        std::map<std::string, bool> my_move_map;

        for (const Move& move : my_moves) {
            std::string move_str = Square_To_String(move.Get_From()) + Square_To_String(move.Get_To());
            if (move.Get_Promotion_Piece() != NO_PIECE) {
                move_str += Get_Piece_Symbol(move.Get_Promotion_Piece());
            }
            my_move_map[move_str] = true;
        }

        for (const Move& move : stockfish_moves) {
            std::string move_str = Square_To_String(move.Get_From()) + Square_To_String(move.Get_To());
            stockfish_move_map[move_str] = true;
        }

        std::cout << "Moves in our engine that aren't in Stockfish:" << std::endl;
        int extra_count = 0;
        for (const auto& [move_str, _] : my_move_map) {
            if (stockfish_move_map.find(move_str) == stockfish_move_map.end()) {
                std::cout << "  " << move_str << std::endl;
                extra_count++;
            }
        }
        if (extra_count == 0) {
            std::cout << "  None" << std::endl;
        } else {
            std::cout << "  Total: " << extra_count << " extra moves" << std::endl;
        }

        // Find moves in stockfish but not in our engine
        std::cout << "\nMoves in Stockfish that aren't in our engine:" << std::endl;
        int missing_count = 0;
        for (const auto& [move_str, _] : stockfish_move_map) {
            if (my_move_map.find(move_str) == my_move_map.end()) {
                std::cout << "  " << move_str << std::endl;
                missing_count++;
            }
        }
        if (missing_count == 0) {
            std::cout << "  None" << std::endl;
        } else {
            std::cout << "  Total: " << missing_count << " missing moves" << std::endl;
        }

        // Print summary
        std::cout << "\nSummary:" << std::endl;
        std::cout << "  Our engine:  " << my_move_map.size() << " moves" << std::endl;
        std::cout << "  Stockfish:   " << stockfish_moves.size() << " moves" << std::endl;
        std::cout << "  Extra:       " << extra_count << " moves" << std::endl;
        std::cout << "  Missing:     " << missing_count << " moves" << std::endl;
    }

};