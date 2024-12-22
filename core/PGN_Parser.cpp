#include "PGN_Parser.h"

#include <fstream>
#include <sstream>
#include "../Moves/Move_Generation.h"
#include <regex>


std::vector<PGN_Game> PGN_Parser::Parse_PGN_File(const std::string &filename) {
    std::vector<PGN_Game> games;
    std::ifstream file(filename);
    if (!file) { return games; }

    std::string line;
    std::string gameString;
    PGN_Game currentGame;

    while (std::getline(file, line)) {
        if (line.empty()) { continue; }

        // Properly check if line contains [Result
        if (line.find("[Result") != std::string::npos) {
            std::string line1, line2;
            std::getline(file, line1);
            std::getline(file, line2);
            std::string gameText = line + "\n";
                        gameText += line1 + "\n";
                        gameText += line2;
            currentGame = Parse_Result_Elo(gameText);

        }
        if (std::isdigit(line[0]) && line.find(".") != std::string::npos) {
            currentGame = Parse_Single_Game(currentGame, line);
        }
    }
    return games;
}

PGN_Game PGN_Parser::Parse_Result_Elo(const std::string &gameText) {
    PGN_Game game;

    std::stringstream ss(gameText);
    std::string line1, line2, line3;
    std::getline(ss, line1);
    std::getline(ss, line2);
    std::getline(ss, line3);

    if (line1.find("0-1") != std::string::npos) {
        game.result = "Black win";
    }
    else if (line1.find("1-0") != std::string::npos) {
        game.result = "White win";
    }
    else {
        game.result = "Draw";
    }

    size_t white_quote_start = line2.find("\"");
    size_t white_quote_end = line2.find("\"", white_quote_start + 1);

    if (white_quote_start != std::string::npos && white_quote_end != std::string::npos) {
        std::string white_rating = line2.substr(white_quote_start + 1, white_quote_end - white_quote_start - 1);
        if (!white_rating.empty()) {
            try {
                game.whiteElo = std::stoi(white_rating);
            } catch (...) {
                game.whiteElo = 0;
            }
        } else {
            game.whiteElo = 0;
        }
    }

    size_t black_quote_start = line3.find("\"");
    size_t black_quote_end = line3.find("\"", black_quote_start + 1);

    if (black_quote_start != std::string::npos && black_quote_end != std::string::npos) {
        std::string black_rating = line3.substr(black_quote_start + 1, black_quote_end - black_quote_start - 1);
        if (!black_rating.empty()) {
            try {
                game.blackElo = std::stoi(black_rating);
            } catch (...) {
                game.blackElo = 0;
            }
        } else {
            game.blackElo = 0;
        }
    }

    return game;
}


PGN_Game PGN_Parser::Parse_Single_Game(PGN_Game game, const std::string& gameText) {
    size_t pos = 0;
    std::stringstream ss(gameText);
    std::string token;
    game.board.Initialise_From_Fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    PieceColour currentTurn = game.board.moveHistory.size() % 2 == 0 ? WHITE : BLACK;

    while (ss >> token) {

        if (token == "1-0" || token == "0-1" || token == "1/2-1/2" || token == "*") {
            continue;
        }

        if (std::isdigit(token[0])) {
            size_t dotPos = token.find(".");
            if (dotPos != std::string::npos && dotPos + 1 < token.length()) {
                token = token.substr(dotPos + 1);
            }
        }
        Parse_Single_Move(game, token);

    }

    return game;

}

void PGN_Parser::Parse_Single_Move(PGN_Game& game, const std::string &gameText) {
    // PAWN
    const PieceColour colour = game.board.moveHistory.size() % 2 == 0 ? WHITE : BLACK;
    if (gameText.length() == 2) {
        const Squares destination = String_To_Square(gameText);

        U64 pawns = game.board.Get_Piece_Bitboard(PAWN, colour);
        pawns &= FILES[destination % 8];

        while (pawns) {
            int position = Get_LS1B_Index(pawns);
            pawns &= -1;

            if (MoveGeneration::Get_Pawn_Moves(position, colour) & (1ULL << destination)) {
                const Move move(static_cast<Squares>(position), destination, PAWN, colour);
                game.board.moveHistory.push_back(move);
                return;
            }
        }
    }
    PieceType piece = NO_PIECE;
    std::string pieceMoveDetails;
    switch (gameText[0]) {
        case 'N':
            piece = KNIGHT;
            pieceMoveDetails = gameText.substr(1, gameText.length());
            break;

        case 'B':
            piece = BISHOP;
            pieceMoveDetails = gameText.substr(1, gameText.length());
            break;

        case 'R':
            piece = ROOK;
            pieceMoveDetails = gameText.substr(1, gameText.length());
            break;

        case 'Q':
            piece = QUEEN;
            pieceMoveDetails = gameText.substr(1, gameText.length());
            break;

        case 'K':
            piece = KING;
            pieceMoveDetails = gameText.substr(1, gameText.length());
            break;

        default:
            pieceMoveDetails = gameText;
            piece = PAWN;
    }

    if (pieceMoveDetails.back() == '+') {
        pieceMoveDetails.pop_back();
    }

    pieceMoveDetails = std::regex_replace(pieceMoveDetails, std::regex("x"),"");

    if (pieceMoveDetails.length() == 2) {
        const Squares destination = String_To_Square(pieceMoveDetails);
        U64 pieceBB = game.board.Get_Piece_Bitboard(piece, colour);

        while (pieceBB) {
            int position = Get_LS1B_Index(pieceBB);
            pieceBB &= pieceBB - 1;
            const U64 pieceMovement = MoveGeneration::Get_Piece_Attacks(piece, position, colour);

            if (pieceMovement & (1ULL << destination)) {
                Move move(static_cast<Squares>(position), destination, piece, colour);
                game.board.moveHistory.push_back(move);
                game.board.Print_Move_Details(move);
                return;
            }
        }
    }
    std::cout << pieceMoveDetails << std::endl;

}
