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
public:
    static void Stockfish_Perft(const std::string &fen, const std::string &depth) {
        std::string cmd = "printf 'position fen " + fen + "\ngo perft " + depth + "\n' | ../stockfish";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            throw std::runtime_error("Failed to open stockfish");
        }
        Board board;
        board.Initialise_From_Fen(fen);
        std::vector<Move> stockfish_moves;
        char buffer[4096];
        std::ofstream fout("../stockfish.txt");
        while (fgets(buffer, sizeof(buffer), pipe)) {
            std::string line(buffer);
            if (line.find("info") != std::string::npos ||
                line.find("Stockfish") != std::string::npos ||
                line.find("NNUE") != std::string::npos) {
                continue;
                }
            fout << line;
            if (line.find(":") != std::string::npos) {
                Move temp_move = Game::Parse_Move(line.substr(0,4), board.current_turn, board);
                if (temp_move != Move()) { stockfish_moves.push_back(temp_move); }
            }
        }
        pclose(pipe);
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
    }
};
