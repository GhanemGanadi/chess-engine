#include "PGN_Parser.h"

#include <fstream>
#include <regex>
#include <sstream>
#include "../Board/Zobrist.h"
#include <unordered_map>

#include "../Moves/Move_Generation.h"

#include "../engine/Evaluation.h"

std::vector<PGN_Game> PGN_Parser::Parse_PGN_File(const std::string &filename) {
    std::vector<PGN_Game> games;
    std::ifstream file(filename);
    if (!file) { return games; }

    std::string line;
    std::string gameString;
    PGN_Game currentGame = PGN_Game();
    bool gameInProgress = false;

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
            currentGame = Parse_Result_Elo(currentGame, gameText);
            // std::cout << "------- NEW GAME -------" << std::endl;
            gameInProgress = true;

        }
        if (std::isdigit(line[0]) && line.find('.') != std::string::npos) {
            currentGame = Parse_Single_Game(currentGame, line);

            if (line.find("1-0") != std::string::npos ||
                line.find("0-1") != std::string::npos ||
                line.find("1/2-1/2") != std::string::npos ||
                line.find("*") != std::string::npos) {

                games.push_back(currentGame);
                currentGame = PGN_Game();
                gameInProgress = false;
            }
        }
    }
    return games;
}

PGN_Game PGN_Parser::Parse_Result_Elo(PGN_Game& game, const std::string &gameText) {
    std::stringstream ss(gameText);
    std::string line1, line2, line3;
    std::getline(ss, line1);
    std::getline(ss, line2);
    std::getline(ss, line3);

    if (line1.find("0-1") != std::string::npos) {
        game.result = -1;
    }
    else if (line1.find("1-0") != std::string::npos) {
        game.result = 1;
    }
    else if (line1.find("1/2-1/2") != std::string::npos) {
        game.result = -0.5;
    }

    size_t white_quote_start = line2.find('\"');
    size_t white_quote_end = line2.find('\"', white_quote_start + 1);

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

    size_t black_quote_start = line3.find('\"');
    size_t black_quote_end = line3.find('\"', black_quote_start + 1);

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
    // PieceColour currentTurn = game.board.moveHistory.size() % 2 == 0 ? WHITE : BLACK;

    while (ss >> token) {

        if (token == "1-0" || token == "0-1" || token == "1/2-1/2" || token == "*") {
            continue;
        }

        if (std::isdigit(token[0])) {
            if (size_t dotPos = token.find('.'); dotPos != std::string::npos && dotPos + 1 < token.length()) {
                token = token.substr(dotPos + 1);
            }
        }
        Parse_Single_Move(game, token);
        // game.board.Print_Detailed_Board();

    }
    return game;
}

PieceType Piece_Identifier(const char& token) {
    switch (token) {
        case 'N':
            return KNIGHT;

        case 'B':
            return BISHOP;

        case 'R':
            return ROOK;

        case 'Q':
            return QUEEN;

        case 'K':
            return KING;

        default:
            return PAWN;
    }
}

int Get_File_Index_From_Move(const char& moveStr) {

    const char file = std::tolower(moveStr);
    if (file >= 'a' && file <= 'h') {
        return file - 'a';
    }

    return -1;
}

U64 Get_Identifying_Mask(std::string& moveStr) {
    U64 identifyingMask = ~0ULL;
    while (!moveStr.empty()) {
        const char rankOrFile = moveStr.front();
        moveStr = moveStr.substr(1, moveStr.length() - 1);
        if (std::isdigit(rankOrFile)) {
            identifyingMask &= RANKS[(rankOrFile - '0') - 1];
        }
        else if (std::isalpha(rankOrFile)) {
            identifyingMask &= FILES[Get_File_Index_From_Move(rankOrFile)];
        }
    }
    return identifyingMask;
}


void Parse_Promotion(PGN_Game& game, std::string &gameText) {
    const PieceColour colour = game.board.moveHistory.size() % 2 == 0 ? WHITE : BLACK;

    const PieceType promotionPiece = Piece_Identifier(gameText.back());
    gameText = gameText.substr(0, gameText.length() - 2);

    const Squares destination = String_To_Square(gameText.substr(gameText.length() - 2));
    gameText = gameText.substr(0, gameText.length() - 2);

    const U64 identifyingMask = Get_Identifying_Mask(gameText);

    U64 pieceBB = game.board.Get_Piece_Bitboard(PAWN, colour) & identifyingMask;

    int position = 0;
    while (pieceBB) {
        position = Get_LS1B_Index(pieceBB);
        pieceBB &= pieceBB - 1;
        const U64 pieceMovement = MoveGeneration::Get_Piece_Attacks(PAWN, position, colour, game.board);
        if (pieceMovement & (1ULL << destination)) {
            break;
        }
    }

    Move move(static_cast<Squares>(position), destination, PAWN, colour);
    Board_Analyser::Promote_Pawn(move, promotionPiece, game.board);
    const U64 enemyBB = colour == WHITE ? game.board.Get_Black_Pieces() : game.board.Get_White_Pieces();
    if (1ULL << destination & enemyBB) {
        Board_Analyser::Handle_Captures(move, game.board);
    }
    Board_Analyser::Move_Piece(move, game.board);
    game.board.moveHistory.push_back(move);

}

void PGN_Parser::Parse_Single_Move(PGN_Game& game, std::string &gameText) {

    std::string possibleWrong = gameText;
    // std::cout << "PARSING MOVE: " << gameText << std::endl;
    // std::cout << "Move number: " << game.board.moveHistory.size() << std::endl;

    const PieceColour colour = game.board.moveHistory.size() % 2 == 0 ? WHITE : BLACK;

    gameText = std::regex_replace(gameText, std::regex("\\+"),"");

    if (gameText == "O-O") {
        Move move(colour == WHITE ? e1 : e8, colour == WHITE ? g1 : g8, KING, colour);
        Board_Analyser::Make_Move(move, false, game.board);
        // game.board.Print_Move_Details(move);
        return;
    }
    if (gameText == "O-O-O") {
        Move move(colour == WHITE ? e1 : e8, colour == WHITE ? c1 : c8, KING, colour);
        Board_Analyser::Make_Move(move, false, game.board);
        // game.board.Print_Move_Details(move);
        return;
    }

    const PieceType piece = Piece_Identifier(gameText[0]);

    if (piece != PAWN) {
        gameText = gameText.substr(1, gameText.length() - 1);
    }

    gameText = std::regex_replace(gameText, std::regex("x"), "");

    if (gameText.find('=') != std::string::npos) {
        Parse_Promotion(game, gameText);
        return;
    }

    const Squares destination = String_To_Square(gameText.substr(gameText.length() - 2));
    gameText = gameText.substr(0, gameText.length() - 2);

    const U64 identifyingMask = Get_Identifying_Mask(gameText);

    U64 pieceBB = game.board.Get_Piece_Bitboard(piece, colour) & identifyingMask;
    int position = 0;

    while (pieceBB) {
        position = Get_LS1B_Index(pieceBB);
        pieceBB &= pieceBB - 1;
        U64 pieceMovement = Engine::moveGen.Get_Legal_Moves(position, colour, piece, game.board);
        if (pieceMovement & (1ULL << destination)) {
            break;
        }

    }
    // const auto position = static_cast<Squares>(Get_LS1B_Index(pieceBB));

    Move move(static_cast<Squares>(position), destination, piece, colour);

    if (Board_Analyser::Make_Move(move, false, game.board)) return;

    std::cout << "PARSING MOVE: " << possibleWrong << std::endl;
    std::cout << game.board.moveHistory.size() << " TAWEAKISFNASDFHIAJDKFAHDJFA       " << gameText << std::endl;

}



