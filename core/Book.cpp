#include "Book.h"

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>

#include "../Board/Board_Analyser.h"
#include "../Board/Zobrist.h"
#include "../config.h"


std::string Move_To_String(Move move) {
    std::string moveString;

    moveString += Square_To_String(move.Get_From());
    moveString += Square_To_String(move.Get_To());

    return moveString;
}

bool OpeningBook::Has_Position(const std::string& fen) const {
    return positions.contains(fen);
}

bool OpeningBook::Has_Move(const std::string& fen, const Move& move) {
    std::string moveString = Move_To_String(move);
    std::vector<MoveStats>& stats = positions[fen];

    for (const MoveStats& stat : stats) {
        if (stat.move == moveString){ return true;}
    }
    return false;
}

void OpeningBook::Process_Games(std::vector<PGN_Game> &games) {

    for (PGN_Game &game : games) {
        Board currentGame;
        currentGame.Initialise_From_Fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        for (size_t moveIndex = 0; moveIndex < std::min(size_t(20), game.board.moveHistory.size());
                                            moveIndex++) {

            std::string fen = currentGame.Board_To_Fen();

            Move& move = game.board.moveHistory[moveIndex];

            if (!Has_Move(fen, move)) {
                MoveStats move_stats = MoveStats(Move_To_String(move), game.result);
                positions[fen].push_back(move_stats);
            }
            else {
                std::vector<MoveStats>& stats = positions[fen];
                for (MoveStats& stat : stats) {
                    if (stat.move == Move_To_String(move)) {
                        stat.results += game.result;
                    }
                }
            }

            if (move.Get_Promotion_Piece() != NO_PIECE) {
                auto promotionMove = Move(move.Get_From(), move.Get_To(), PAWN, move.Get_Colour());
                Board_Analyser::Promote_Pawn(promotionMove, move.Get_Promotion_Piece(), currentGame);
                Board_Analyser::Move_Piece(move, currentGame);
                currentGame.moveHistory.push_back(move);
                continue;
            }

            Board_Analyser::Make_Move(move, true, currentGame);

            // std::cout << Square_To_String(move.Get_From()) << " " << Square_To_String(move.Get_To())
                    // << " " << Get_Piece_Name(move.Get_Piece_Type()) << std::endl;
        }
    }
}


void OpeningBook::Save_To_File(std::string filename) const {
    Zobrist zobrist;
    zobrist.Initialise();

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    for (const auto& [fen, movesList] : positions) {
        file << fen << std::endl;
        auto sortedMoves = movesList;
        std::ranges::sort(sortedMoves, [](const MoveStats& a, const MoveStats& b) {
            return a.results > b.results;
        });

        for (const auto& moveStats : sortedMoves) {
            file << moveStats.move << ":" << moveStats.results << " ";
        }
        file << std::endl;
    }

    file.close();
}

Move Find_Highest_Result(std::string& moveList) {
    std::istringstream ss(moveList);
    std::string token;
    double maxResult = -std::numeric_limits<double>::infinity();
    Move bestMove(a8, a8, NO_PIECE, NO_COLOUR);
    while (std::getline(ss, token, ' ')) {
        token = std::regex_replace(token, std::regex("\\:"),"");
        Squares position = String_To_Square(token.substr( 0,2));
        token = token.substr(2);
        Squares destination = String_To_Square(token.substr(0,2));
        token = token.substr(2);

        double result = std::stod(token);
        if (result > maxResult) {
            maxResult = result;
            bestMove.Set_From(position);
            bestMove.Set_To(destination);
        }
    }

    return bestMove;
}


Move OpeningBook::Get_Book_Move(const Board &position) {
    std::string fen = position.Board_To_Fen();
    std::string line;

    std::ifstream file(OPENING_BOOK_PATH);

    while (std::getline(file, line)) {
        if (line.find(fen) != std::string::npos) {
            std::cout << "LINE FOUND: " << line << std::endl;
            std::getline(file, line);
            return Find_Highest_Result(line);
        }
    }

}


