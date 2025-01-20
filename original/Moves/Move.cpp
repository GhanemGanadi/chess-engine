#include "Move.h"

#include "../Board/Board.h"
#include "../Board/Board_Analyser.h"


namespace Move_Parsing {

    Move Parse_Move(const std::string& input, const PieceColour colour, const Board& board) {
        Move userMove = Move(a8,a8, NO_PIECE, colour);
        const std::string fromSquare = input.substr(0, 2);
        const std::string toSquare = input.substr(2, 2);

        if(!Check_Move(fromSquare, toSquare)) {return userMove;}

        const auto from = static_cast<Squares>(('8' - fromSquare[1]) * 8 + (fromSquare[0] - 'a'));
        const auto to = static_cast<Squares>(('8' - toSquare[1]) * 8 + (toSquare[0] - 'a'));
        const PieceType piece = Board_Analyser::Find_Piece_Type(from, colour, board);

        userMove.Set_To(to);
        userMove.Set_From(from);
        userMove.Set_Piece_Type(piece);

        if (piece == ROOK) {
            if ((1ULL << from & board.whiteKingRook) ||
                (1ULL << from & board.blackKingRook)) { userMove.Set_King_Side_Castle(true); }

            else {
                if ((1ULL << from & board.whiteQueenRook) ||
                    (1ULL << from & board.blackQueenRook)) { userMove.Set_King_Side_Castle(false); }
            }
        }

        return userMove;
    }

    bool Check_Move(const std::string& fromSquare, const std::string& toSquare){
        if(fromSquare.length() != 2 || toSquare.length() != 2){return false;}

        char firstFile = fromSquare[0];
        char firstRank = fromSquare[1];

        if(firstFile < 'a' || firstFile > 'h') {return false;}
        if(firstRank < '1' || firstRank > '8') {return false;}

        char secondFile = toSquare[0];
        char secondRank = toSquare[1];
        if(secondFile < 'a' || secondFile > 'h') {return false;}
        if(secondRank < '1' || secondRank > '8') {return false;}

        return true;
    }
}
