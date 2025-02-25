#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

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

    void Test(const std::string &fen, const std::string& depth) {
        std::ofstream stockfish_output("../stockfish.txt");
        Send_Command("position fen " + fen);
        Send_Command("go perft " + depth);

        std::string line;
        do {
            line = Get_Response();
            // std::cout << line;
            stockfish_output <<  line;
        } while (line.find("Nodes searched") == std::string::npos);

        stockfish_output.close();
    }


};