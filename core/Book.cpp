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

bool OpeningBook::Has_Position(const U64 hash) const {
    return positions.contains(hash);
}

bool OpeningBook::Has_Move(const U64 hash, const Move& move) {
    std::string moveString = Move_To_String(move);
    std::vector<MoveStats>& stats = positions[hash];

    for (const MoveStats& stat : stats) {
        if (stat.move == moveString) {
            return true;
        }
    }
    return false;
}

void OpeningBook::Process_Games(std::vector<PGN_Game> &games) {

    for (PGN_Game &game : games) {
        U64 currentGameHash = zobrist.Calculate_Hash_Initial_Position();

        for (size_t moveIndex = 0; moveIndex < std::min(size_t(20), game.board.moveHistory.size());
                                            moveIndex++) {


            Move& move = game.board.moveHistory[moveIndex];

            if (!Has_Move(currentGameHash, move)) {
                MoveStats move_stats = MoveStats(Move_To_String(move), game.result);
                positions[currentGameHash].push_back(move_stats);
            }
            else {
                std::vector<MoveStats>& stats = positions[currentGameHash];
                for (MoveStats& stat : stats) {
                    if (stat.move == Move_To_String(move)) {
                        stat.results += game.result;
                    }
                }
            }

            currentGameHash = zobrist.Zobrist_Make_Move(move, currentGameHash);

            // std::cout << Square_To_String(move.Get_From()) << " " << Square_To_String(move.Get_To())
                    // << " " << Get_Piece_Name(move.Get_Piece_Type()) << std::endl;
        }
    }
}


void OpeningBook::Save_To_File(const std::string& filename) const {

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    for (const auto& [hash, movesList] : positions) {
        file << hash << std::endl;
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

BestMoveStat Find_Highest_Result(const std::string& moveList) {
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
    return BestMoveStat(bestMove, maxResult);
}


BestMoveStat OpeningBook::Get_Book_Move(const Board &position) {
    U64 hash = zobrist.Calculate_Hash(position);
    std::string line;
    std::ifstream file(OPENING_BOOK_PATH);

    while (std::getline(file, line)) {

        if (line.find(':') != std::string::npos) { continue; }
        U64 fileHash = std::stoull(line);

        if (fileHash == hash) {
            std::getline(file, line);
            return Find_Highest_Result(line);
        }
    }

    return BestMoveStat(Move(a8, a8, NO_PIECE, NO_COLOUR), 0);
}


