#include "Game.h"
#include "Move_Generator.h"

Move Game::Parse_Move(const std::string& move, PieceColour colour, const Board& board) {
    Move user_move = Move();
    const std::string from_square = move.substr(0, 2);
    const std::string to_square = move.substr(2, 2);

    if(!Check_Move(from_square, to_square)) {return user_move;}

    const int from = ('8' - from_square[1]) * 8 + (from_square[0] - 'a');
    const int to = ('8' - to_square[1]) * 8 + (to_square[0] - 'a');
    const PieceType piece = board.Get_Piece_At_Square(from, colour);

    user_move.Set_To(to);
    user_move.Set_From(from);
    user_move.Set_Piece(piece);
    user_move.Set_Colour(colour);

    return user_move;
}

bool Game::Check_Move(const std::string& from_square, const std::string& to_square){
    if(from_square.length() != 2 || to_square.length() != 2){return false;}

    const char first_file = from_square[0];
    const char first_rank = from_square[1];

    if(first_file < 'a' || first_file > 'h') {return false;}
    if(first_rank < '1' || first_rank > '8') {return false;}

    const char second_file = to_square[0];
    const char second_rank = to_square[1];
    if(second_file < 'a' || second_file > 'h') {return false;}
    if(second_rank < '1' || second_rank > '8') {return false;}

    return true;
}



void Game::Play_Game(const std::string& fen) {
    Board board;
    board.Initialise_From_Fen(fen);
    std::string turn;
    std::string user_move_string;
    while (true) {
        turn = board.current_turn == WHITE ? "White " : "Black ";
        board.Print_Detailed_Board();
        std::vector<Move> moves = Move_Generator::Generate_All_Moves(board);
        Move_Generator::Display_All_Moves(moves);

        std::cout << turn << "to make move OR undo move: ";
        std::cin >> user_move_string;
        if (user_move_string == "undo") {
            board.Undo_Move();
            continue;
        }

        Move user_move = Parse_Move(user_move_string, board.current_turn, board);

        if (Move_Generator::Make_Move(user_move, false, board)) {
            std::cout << "Move successful" << std::endl;
        }

        else { std::cout << "Move failed" << std::endl; }



    }
}
