#pragma once
#include <string>
#include "../Board/Board.h"


struct PGN_Game {
    Board board;
    double result = 0;
    int whiteElo = 0;
    int blackElo = 0;

    PGN_Game() {
        board.Initialise_From_Fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
   }
};

class PGN_Parser {
    public:


    static std::vector<PGN_Game> Parse_PGN_File(const std::string& filename);
    static PGN_Game Parse_Single_Game(PGN_Game game, const std::string& gameText);
    static PGN_Game Parse_Result_Elo(PGN_Game& game, const std::string& gameText);
    static void Parse_Single_Move(PGN_Game& game, std::string& gameText);

};