#include "perft.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>

void Debug_Perft_State(const Board& board, const std::string& message) {
    // Check if the most recent move in history is h1h7 by white
    // or if we're in a line of play that started with h1h7
    bool isRelevant = false;

    if (!board.moveHistory.empty()) {
        const Move& firstMove = board.moveHistory[0];
        if (firstMove.Get_From() == h1 && firstMove.Get_To() == h7 &&
            firstMove.Get_Piece_Type() == ROOK && firstMove.Get_Colour() == WHITE) {
            isRelevant = true;
        }
    }

    if (!isRelevant) return;  // Skip logging if not relevant to h1h7

    for(const auto& move : board.moveHistory) {
        std::cout << Square_To_String(move.Get_From()) << Square_To_String(move.Get_To());
        if(move.Is_Capture()) std::cout << " (capture)";
        if(move.Is_Castling()) std::cout << " (castle)";
        if(move.Is_En_Passant()) std::cout << " (en passant)";
        if(move.Get_Promotion_Piece() != NO_PIECE) {
            std::cout << " (promotion to " << Get_Piece_Name(move.Get_Promotion_Piece()) << ")";
        }
        std::cout << "\n";
    }

    // Print the current board state
    std::cout << "\nCurrent Board Position:\n";
    board.Print_Detailed_Board();

    std::cout << "========================\n\n";
}

int Perft::Run_Perft(Board& board, const int depth) {
    if (depth == 0) { return 1; }

    int nodes = 0;
    std::vector<Move> allMoves = MoveGeneration::Generate_All_Moves(board.currentTurn, board);

    for (Move& move : allMoves) {
        if (Board_Analyser::Make_Move(move, true, board)) {
            nodes += Run_Perft(board, depth - 1);
            board.Undo_Move(false);
        }
    }

    return nodes;
}

void Perft::Perft_Divide(Board& board, const int depth) {
    std::vector<Move> allMoves = MoveGeneration::Generate_All_Moves(board.currentTurn, board);
    int totalNodes = 0;
    for (Move& move : allMoves) {
        if (Board_Analyser::Make_Move(move, true, board)) {
            int nodes = Run_Perft(board, depth - 1);
            std::cout << Square_To_String(move.Get_From()) << Square_To_String(move.Get_To());

            if (move.Is_Capture()) std::cout << " capture";
            if (move.Is_En_Passant()) std::cout << " ep";
            if (move.Is_Castling()) std::cout << " castle";
            if (move.Get_Promotion_Piece() != NO_PIECE) {
                std::cout << " promotion=" << Get_Piece_Name(move.Get_Promotion_Piece());
            }

            std::cout << ": " << nodes << " nodes\n";
            totalNodes += nodes;

            board.Undo_Move(false);

        }
    }

    std::cout << "\nTotal nodes: " << totalNodes << "\n";
}

void Perft::Perft_Divide_Debugging(Board& board, const int depth) {
    std::vector<Move> allMoves = MoveGeneration::Generate_All_Moves(board.currentTurn, board);
    MoveGeneration::Display_All_Moves(allMoves);
    std::string turnString, input;

    std::cout << turnString;
    std::cout << "Input your move (or 'undo' to take back last move): ";
    std::cin >> input;

    Move userMove = Move_Parsing::Parse_Move(input, board.currentTurn, board);

    if (Board_Analyser::Make_Move(userMove, false, board)) {
        Perft_Divide(board, depth - 1);
        std::cout << board.Board_To_Fen() << std::endl;
    }
}


PerftStats Perft::Run_Perft_Stats(Board& board, const int depth, const PieceColour side) {
    const PieceColour enemyColour = side == WHITE ? BLACK : WHITE;
    PerftStats stats;

    if (depth == 0) {
        stats.nodes = 1;
        return stats;
    }

    std::vector<Move> allMoves = MoveGeneration::Generate_All_Moves(side, board);
    if (allMoves.empty()) return stats;

    for (Move& singleMove : allMoves) {
        if (Board_Analyser::Make_Move(singleMove, true, board)) {
            PerftStats subStats = Run_Perft_Stats(board, depth - 1, enemyColour);
            stats.Add(subStats);

            if (depth == 1) {
                if (singleMove.Is_Capture()) stats.captures++;
                if (singleMove.Is_En_Passant()) stats.enPassant++;
                if (singleMove.Is_Castling()) stats.castles++;
                if (singleMove.Get_Promotion_Piece() != NO_PIECE) stats.promotions++;

                U64 enemyKing = board.Get_Piece_Bitboard(KING, enemyColour);
                if (Board_Analyser::Is_King_In_Check(enemyKing, enemyColour, board) != -1) {
                    stats.checks++;
                    if (Board_Analyser::Is_Checkmate(enemyColour, board)) {
                        stats.checkmates++;
                    }
                }
            }

            board.Undo_Move(false);
        }
    }

    return stats;
}
