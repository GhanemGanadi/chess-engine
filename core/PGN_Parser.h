#pragma once
#include <string>
#include "../Board/Board.h"

struct PGN_Game {
    std::vector<Move> moves;
    std::string result;
    int whiteElo = 0;
    int blackElo = 0;
};

class PGN_Parser {


    static std::vector<PGN_Game> Parse_PGN_File(const std::string& filename);

};