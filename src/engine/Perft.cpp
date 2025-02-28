#include "Perft.h"

#include <fstream>
#include <map>

#include "Game.h"


int Perft::Run_Perft(Board &board, int depth) {
    if (depth == 0) { return 1;}

    int nodes = 0;
    std::vector<Move> all_moves = Move_Generator::Generate_All_Moves(board);

    for (Move& move : all_moves) {
        if (Move_Generator::Make_Move(move, true, board)) {
            std::cout << board.Board_To_Fen() << std::endl;
            nodes += Run_Perft(board, depth - 1);
            board.Undo_Move();
        }
    }
    return nodes;
}

void Perft::Perft_Divide(Board& board, const int depth) {
    std::ofstream outFile("../output.txt");
    std::vector<Move> all_moves = Move_Generator::Generate_All_Moves(board);

    int total_nodes = 0;
    for (Move& move : all_moves) {
        if (Move_Generator::Make_Move(move, true, board)) {
            std::cout << board.Board_To_Fen() << std::endl;
            int nodes = Run_Perft(board, depth - 1);
            // std::cout << Square_To_String(move.Get_From()) << Square_To_String(move.Get_To());
            outFile << Square_To_String(move.Get_From()) << Square_To_String(move.Get_To());

            if (move.Get_Promotion_Piece() != NO_PIECE) {
                // std::cout << Get_Piece_Symbol(move.Get_Promotion_Piece());
                outFile << Get_Piece_Symbol(move.Get_Promotion_Piece());
            }

            // std::cout << ": " << nodes << std::endl;
            outFile << ": " << nodes << std::endl;
            total_nodes += nodes;

            board.Undo_Move();
        }

    }
    // std::cout << "Nodes searched: " << total_nodes << std::endl;
    outFile << "Nodes searched: " << total_nodes << std::endl;
    outFile.close();
}

void Perft::Perft_Divide_Debugging(const std::string& fen, int depth) {
    std::string turn_string, input;
    Board board;
    board.Initialise_From_Fen(fen);
    board.Print_Detailed_Board();

    std::cout << turn_string;
    std::cout << "Input your move (or 'undo' to take back last move): ";
    std::cin >> input;

    Move user_move = Game::Parse_Move(input, board.current_turn, board);

    if (Move_Generator::Make_Move(user_move, false, board)) {
        Perft_Divide(board, depth - 1);
    }

}

void Perft::Analyse_Differences(const std::string& my_output_file, const std::string& stockfish_file) {
    std::ifstream my_file(my_output_file);
    std::map<std::string, int> my_moves;
    std::string line;

    while (std::getline(my_file, line)) {
        if (line.find(":") != std::string::npos && line.find("Nodes searched:") == std::string::npos) {
            std::string move = line.substr(0, line.find(":"));
            int count = std::stoi(line.substr(line.find(":") + 2));
            my_moves[move] = count;
        }
    }

    std::ifstream stock_file(stockfish_file);
    std::map<std::string, int> stockfish_moves;

    while (std::getline(stock_file, line)) {
        if (line.find(":") != std::string::npos && line.find("Nodes searched:") == std::string::npos) {
            std::string move = line.substr(0, line.find(":"));
            int count = std::stoi(line.substr(line.find(":") + 2));
            stockfish_moves[move] = count;
        }
    }

    std::cout << "Missing moves (in Stockfish but not mine):\n";
    for (const auto& [move, count] : stockfish_moves) {
        if (my_moves.find(move) == my_moves.end()) {
            std::cout << move << ": " << count << " nodes\n";
        }
    }

    std::cout << "\nExtra moves (in mine but not in Stockfish):\n";
    for (const auto& [move, count] : my_moves) {
        if (stockfish_moves.find(move) == stockfish_moves.end()) {
            std::cout << move << ": " << count << " nodes\n";
        }
    }

    std::cout << "\nNode count differences:\n";
    std::cout << "MOVE |  ME  |  SF  |\n";
    for (const auto& [move, count] : stockfish_moves) {
        if (my_moves.find(move) != my_moves.end() && my_moves[move] != count) {
            std::cout << move << " | " << my_moves[move] << " | " << count << " |"
                     << "    (diff: " << my_moves[move] - count << ")\n";
        }
    }
}
