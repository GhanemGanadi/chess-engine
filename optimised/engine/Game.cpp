#include "Game.h"
#include "Move_Generator.h"

Board Game::board;

Move Game::Parse_Move(const std::string& move, PieceColour colour) {
    Move userMove = Move(a8,a8, NO_PIECE, colour);
    const std::string fromSquare = move.substr(0, 2);
    const std::string toSquare = move.substr(2, 2);

    if(!Check_Move(fromSquare, toSquare)) {return userMove;}

    const int from = ('8' - fromSquare[1]) * 8 + (fromSquare[0] - 'a');
    const int to = ('8' - toSquare[1]) * 8 + (toSquare[0] - 'a');
    const PieceType piece = board.Get_Piece_At_Square(from, colour);

    userMove.Set_To(to);
    userMove.Set_From(from);
    userMove.Set_Piece(piece);

    return userMove;
}

bool Game::Check_Move(const std::string& fromSquare, const std::string& toSquare){
    if(fromSquare.length() != 2 || toSquare.length() != 2){return false;}

    const char firstFile = fromSquare[0];
    const char firstRank = fromSquare[1];

    if(firstFile < 'a' || firstFile > 'h') {return false;}
    if(firstRank < '1' || firstRank > '8') {return false;}

    const char secondFile = toSquare[0];
    const char secondRank = toSquare[1];
    if(secondFile < 'a' || secondFile > 'h') {return false;}
    if(secondRank < '1' || secondRank > '8') {return false;}

    return true;
}



void Game::Play_Game(const std::string& fen) {
    board.Initialise_From_Fen(fen);
    std::string turn;
    std::string user_move_string;
    while (true) {
        turn = board.current_turn == WHITE ? "White " : "Black ";
        board.Print_Detailed_Board();

        std::cout << turn << "to make move: ";
        std::cin >> user_move_string;
        Move user_move = Parse_Move(user_move_string, board.current_turn);
        if (Move_Generator::Make_Move(user_move, board)) {
            std::cout << "Move successful" << std::endl;
        }

        else { std::cout << "Move failed" << std::endl; }



    }
}
