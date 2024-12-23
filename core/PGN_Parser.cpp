#include "PGN_Parser.h"

#include <fstream>
#include <sstream>
#include "../Moves/Move_Generation.h"
#include <regex>
#include <typeinfo>

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
            std::cout << "------- NEW GAME -------" << std::endl;
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
        game.result = "Black win";
    }
    else if (line1.find("1-0") != std::string::npos) {
        game.result = "White win";
    }
    else {
        game.result = "Draw";
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
        game.board.Print_Detailed_Board();

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

int Get_File_Index_From_Move(const std::string& moveStr) {
    if (moveStr.empty()) return -1;


    char file = std::tolower(moveStr[0]);
    if (file >= 'a' && file <= 'h') {
        return file - 'a';
    }

    return -1;
}

void Parse_Promotion(PGN_Game& game, const std::string &gameText) {
    std::string pieceMoveDetails = gameText;
    pieceMoveDetails = std::regex_replace(pieceMoveDetails, std::regex("="),"");
    const PieceColour colour = game.board.moveHistory.size() % 2 == 0 ? WHITE : BLACK;

    const PieceType promotionPiece = Piece_Identifier(pieceMoveDetails.back());
    pieceMoveDetails.pop_back();
    U64 identifyingMask = ~0ULL;

    if (std::isdigit(pieceMoveDetails[0])) {
        identifyingMask = RANKS[(pieceMoveDetails[0] - '0') - 1];
    }
    else {
        identifyingMask = FILES[Get_File_Index_From_Move(pieceMoveDetails)];
    }

    if (pieceMoveDetails.length() == 2) {
        const Squares destination = String_To_Square(pieceMoveDetails);
        U64 pieceBB = game.board.Get_Piece_Bitboard(PAWN, colour) & identifyingMask;

        while (pieceBB) {
            int position = Get_LS1B_Index(pieceBB);
            pieceBB &= pieceBB - 1;
            const U64 pieceMovement = MoveGeneration::Get_Piece_Attacks(PAWN, position, colour, game.board);
            // Squares destination = String_To_Square(pieceMoveDetails);

        }



    }

}

void PGN_Parser::Parse_Single_Move(PGN_Game& game, const std::string &gameText) {
    // PAWN
    const PieceColour colour = game.board.moveHistory.size() % 2 == 0 ? WHITE : BLACK;
    std::cout << "Move number: " << game.board.moveHistory.size() << std::endl;
    const PieceType piece = Piece_Identifier(gameText[0]);
    std::string pieceMoveDetails = gameText;
    if (piece != PAWN) {
        pieceMoveDetails = pieceMoveDetails.substr(1, pieceMoveDetails.length() - 1);
    }


    pieceMoveDetails = std::regex_replace(pieceMoveDetails, std::regex("+"),"");
    pieceMoveDetails = std::regex_replace(pieceMoveDetails, std::regex("x"),"");

    if (pieceMoveDetails.find('=') != std::string::npos) {
        Parse_Promotion(game, gameText);
    }


    if (pieceMoveDetails == "O-O") {
        Move move(colour == WHITE ? e1 : e8, colour == WHITE ? g1 : g8, KING, colour);
        Board_Analyser::Make_Move(move, false, game.board);
        game.board.Print_Move_Details(move);
        return;
    }
    if (pieceMoveDetails == "O-O-O") {
        Move move(colour == WHITE ? e1 : e8, colour == WHITE ? c1 : c8, KING, colour);
        Board_Analyser::Make_Move(move, false, game.board);
        game.board.Print_Move_Details(move);
        return;
    }

    if (pieceMoveDetails.length() == 2) {
        const Squares destination = String_To_Square(pieceMoveDetails);
        U64 pieceBB = game.board.Get_Piece_Bitboard(piece, colour);

        while (pieceBB) {
            int position = Get_LS1B_Index(pieceBB);
            pieceBB &= pieceBB - 1;
            const U64 pieceMovement = MoveGeneration::Get_Piece_Attacks(piece, position, colour, game.board);

            if (pieceMovement & (1ULL << destination)) {
                Move move(static_cast<Squares>(position), destination, piece, colour);
                Board_Analyser::Make_Move(move, false, game.board);
                game.board.Print_Move_Details(move);
                return;
            }
        }
    }

    if (pieceMoveDetails.length() == 3) {
        U64 identifyingMask = 0ULL;
        if (std::isdigit(pieceMoveDetails[0])) {
            identifyingMask = RANKS[(pieceMoveDetails[0] - '0') - 1];
        }
        else {
            identifyingMask = FILES[Get_File_Index_From_Move(pieceMoveDetails)];
        }
        pieceMoveDetails = pieceMoveDetails.substr(1, pieceMoveDetails.length() - 1);
        U64 pieceInFile = game.board.Get_Piece_Bitboard(piece, colour) & identifyingMask;

        while (pieceInFile) {
            const int position = Get_LS1B_Index(pieceInFile);
            pieceInFile &= pieceInFile - 1;

            const U64 pieceAttacks = MoveGeneration::Get_Piece_Attacks(piece, position, colour, game.board);
            Squares destination = String_To_Square(pieceMoveDetails);

            if (pieceAttacks & (1ULL << destination)) {
                Move move(static_cast<Squares>(position), destination, piece, colour);
                Board_Analyser::Make_Move(move, false, game.board);
                game.board.Print_Move_Details(move);
                return;
            }
        }
    }
    // dc1=Q
    if (pieceMoveDetails.length() > 3) {
        pieceMoveDetails = std::regex_replace(pieceMoveDetails, std::regex("="),"");
        const PieceType promotionPiece = Piece_Identifier(pieceMoveDetails.back());
        pieceMoveDetails.pop_back();
        U64 identifyingMask = ~0ULL;
        std::string destinationString = pieceMoveDetails.substr(pieceMoveDetails.length() - 2,
                                                                  pieceMoveDetails.length());

        if (std::isdigit(pieceMoveDetails[0])) {
            identifyingMask = RANKS[(pieceMoveDetails[0] - '0') - 1];
        }
        else {
            identifyingMask = FILES[Get_File_Index_From_Move(pieceMoveDetails)];
        }

        Squares destination = String_To_Square(destinationString);
        U64 pieceBB = game.board.Get_Piece_Bitboard(piece, colour) & identifyingMask;

        while (pieceBB) {
            int position = Get_LS1B_Index(pieceBB);
            pieceBB &= pieceBB - 1;
            const U64 pieceMovement = MoveGeneration::Get_Piece_Attacks(piece, position, colour, game.board);

            if (pieceMovement & (1ULL << destination)) {
                Move move(static_cast<Squares>(position), destination, piece, colour);
                const U64 enemyBB = move.Get_Colour() == WHITE ? game.board.Get_Black_Pieces() :
                                                                 game.board.Get_White_Pieces();

                if (enemyBB & (1ULL << move.Get_To())) {
                    Board_Analyser::Handle_Captures(move, game.board);
                }
                Board_Analyser::Promote_Pawn(move, promotionPiece, game.board);
                Board_Analyser::Move_Piece(move, game.board);
                game.board.moveHistory.push_back(move);
                game.board.Print_Move_Details(move);
                return;
            }
        }
    }
    std::cout << game.board.moveHistory.size() << " TAWEAKISFNASDFHIAJDKFAHDJFA       " << pieceMoveDetails << std::endl;

}
