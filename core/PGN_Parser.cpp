#include "PGN_Parser.h"

#include <fstream>


std::vector<PGN_Game> PGN_Parser::Parse_PGN_File(const std::string &filename) {
    std::vector<PGN_Game> games;
    std::ifstream file(filename);
    if (!file) { return games; }

    std::string line;
    std::string currentGame;


    return games;
}
