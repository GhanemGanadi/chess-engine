#include "Perft.h"



int Perft::Run_Perft(Board &board, int depth) {
    if (depth == 0) { return 1;}

    int nodes = 0;
    std::vector<Move> all_moves = Move_Generator::Generate_All_Moves(board);

    for (Move& move : all_moves) {
        if (Move_Generator::Make_Move(move, true, board)) {
            nodes += Run_Perft(board, depth - 1);
            board.Undo_Move();
        }
    }
    return nodes;
}

void Perft::Perft_Divide(const std::string& fen, const int depth) {
    Board board;
    board.Initialise_From_Fen(fen);
    std::vector<Move> all_moves = Move_Generator::Generate_All_Moves(board);

    int total_nodes = 0;

    for (Move& move : all_moves) {

        if (Move_Generator::Make_Move(move, true, board)) {
            int nodes = Run_Perft(board, depth - 1);
            std::cout << Square_To_String(move.Get_From()) << Square_To_String(move.Get_To());

            if (move.Get_Promotion_Piece() != NO_PIECE) {
                std::cout << Get_Piece_Symbol(move.Get_Promotion_Piece());
            }

            std::cout << ": " << nodes << " nodes" << std::endl;
            total_nodes += nodes;

            board.Undo_Move();
        }

    }
    std::cout << "Total nodes: " << total_nodes << std::endl;
}

