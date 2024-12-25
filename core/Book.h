#pragma once
#include <unordered_map>
#include "../Board/Board.h"

#include "PGN_Parser.h"

struct OpeningPosition {
    std::string fen;
    std::vector<Move> moves;
    std::vector<int> counts;
    int whiteWins = 0;
    int blackWins = 0;
    int draws = 0;
};

struct MoveStats {
    std::string move;
    double results;

};

class OpeningBook {

    // Key: FEN string, Value: map of win ratio and the move
    std::unordered_map<std::string, std::vector<MoveStats>> positions;


public:
    void Process_Games(std::vector<PGN_Game>& games);
    void Save_To_File(std::string filename) const;
    Move Get_Book_Move(const Board& position);
    [[nodiscard]] bool Has_Position(const std::string& fen) const;
    [[nodiscard]] bool Has_Move(const std::string& fen, const Move& move);
};