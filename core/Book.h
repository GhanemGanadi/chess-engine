#pragma once
#include <unordered_map>
#include "../Board/Board.h"

#include "../Board/Zobrist.h"
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

struct BestMoveStat {
    Move bestMove;
    double bestResults;
};

class OpeningBook {
    Zobrist zobrist;
    std::unordered_map<U64, std::vector<MoveStats>> positions;


public:
    OpeningBook() {
        zobrist.Initialise();
    }

    void Process_Games(std::vector<PGN_Game>& games);
    void Save_To_File(const std::string& filename) const;
    BestMoveStat Get_Book_Move(const Board& position);
    [[nodiscard]] bool Has_Position(U64 hash) const;
    [[nodiscard]] bool Has_Move(U64 hash, const Move& move);

};