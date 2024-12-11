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

    std::cout << "\n=== " << message << " ===\n";

    // Print castling rights
    std::cout << "Castling Rights:\n"
              << "White Kingside:  " << (board.Has_White_King_Side_Castling_Rights() ? "Yes" : "No") << "\n"
              << "White Queenside: " << (board.Has_White_Queen_Side_Castling_Rights() ? "Yes" : "No") << "\n"
              << "Black Kingside:  " << (board.Has_Black_King_Side_Castling_Rights() ? "Yes" : "No") << "\n"
              << "Black Queenside: " << (board.Has_Black_Queen_Side_Castling_Rights() ? "Yes" : "No") << "\n";

    // Print move history
    std::cout << "\nMove History (" << board.moveHistory.size() << " moves):\n";
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

U64 Perft::Run_Perft(Board& board, const int depth, const PieceColour side) {
    if (depth == 0) return 1;

    U64 nodes = 0;
    std::vector<Move> allMoves = moveGen.Generate_All_Moves(side, board);

    for (Move& move : allMoves) {
        if (Board_Analyser::Make_Move(move, true, board)) {
            // Debug_Perft_State(board, "After Make_Move - Depth " + std::to_string(depth));
            nodes += Run_Perft(board, depth - 1, (side == WHITE ? BLACK : WHITE));
            board.Undo_Move(false);
            // Debug_Perft_State(board, "After Undo_Move - Depth " + std::to_string(depth));
        }
    }

    return nodes;
}

void Perft::Perft_Divide(Board& board, const int depth, const PieceColour side) {
    std::vector<Move> allMoves = moveGen.Generate_All_Moves(side, board);
    int totalNodes = 0;

    for (Move& move : allMoves) {
        // Special detailed logging for h1h7
        // if (move.Get_From() == h1 && move.Get_To() == h7 &&
            // move.Get_Piece_Type() == ROOK && move.Get_Colour() == WHITE) {
            // std::cout << "\n=== Starting analysis of h1h7 ===\n";
            // Debug_Perft_State(board, "Before h1h7");
        // }

        if (Board_Analyser::Make_Move(move, true, board)) {
            int nodes = Run_Perft(board, depth - 1, (side == WHITE ? BLACK : WHITE));
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

            // Special detailed logging for h1h7
            // if (move.Get_From() == h1 && move.Get_To() == h7 &&
                // move.Get_Piece_Type() == ROOK && move.Get_Colour() == WHITE) {
                // Debug_Perft_State(board, "After undoing h1h7");
                // std::cout << "=== Finished analysis of h1h7 ===\n\n";
            // }
        }
    }

    std::cout << "\nTotal nodes: " << totalNodes << "\n";
}


PerftStats Perft::Run_Perft_Stats(Board& board, const int depth, const PieceColour side) {
    const PieceColour enemyColour = side == WHITE ? BLACK : WHITE;
    PerftStats stats;

    if (depth == 0) {
        stats.nodes = 1;  // Count this position as one node
        return stats;
    }

    std::vector<Move> allMoves = moveGen.Generate_All_Moves(side, board);
    if (allMoves.empty()) return stats;

    for (Move& singleMove : allMoves) {
        if (Board_Analyser::Make_Move(singleMove, true, board)) {
            // For non-leaf nodes, accumulate stats from deeper levels
            PerftStats subStats = Run_Perft_Stats(board, depth - 1, enemyColour);
            stats.Add(subStats);

            // Additional stats only counted at depth == 1
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
