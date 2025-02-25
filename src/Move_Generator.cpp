#include "Move_Generator.h"
#include "Attack_Tables.h"

BB Move_Generator::Get_Pseudo_Legal_Moves(const int square, const PieceType piece, const PieceColour colour,
                                                    const Board &board) {

    const BB friendly = colour == WHITE ? board.Get_White_Pieces() : board.Get_Black_Pieces();
    const BB enemy = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();
    const PieceColour enemy_colour = colour == WHITE ? BLACK : WHITE;
    const BB occupancy = friendly | enemy;

    switch (piece) {
        case PAWN:
            return Attack_Tables::Get_Pawn_Moves(square, colour, occupancy) |
                    Attack_Tables::Get_Pawn_Attacks(square, colour, enemy, board.en_passant_square);

        case KNIGHT:
            return Attack_Tables::Get_Knight_Moves(square, friendly);

        case BISHOP:
            return Attack_Tables::Get_Bishop_Moves(square, occupancy) & ~friendly;

        case ROOK:
            return Attack_Tables::Get_Rook_Moves(square, occupancy) & ~friendly;

        case QUEEN:
            return Attack_Tables::Get_Queen_Moves(square, occupancy) & ~friendly;

        case KING:
            return Attack_Tables::Get_King_Moves(square, friendly) |
                   board.Get_Castle_Moves(colour, Attack_Tables::Generate_All_Attacks(enemy_colour, board));
        default:;
    }
    return 0ULL;

}


BB Move_Generator::Get_Legal_Moves(const int square, const PieceType piece, const PieceColour colour,
                                    Board &board) {

    BB moves = Get_Pseudo_Legal_Moves(square, piece, colour, board);
    const int king_square =  Get_LSB(board.Get_Piece(KING, colour));
    const PieceColour enemy_colour = colour == WHITE ? BLACK : WHITE;

    BB checkers = Attack_Tables::Get_Attacks_From_Square(king_square, colour, board);

    /*
     *  WHAT MUST THE FUNCTION DO;
     *      - ONLY ALLOW THE KING TO MOVE TO A SAFE SQUARE
     *          * ALL SQUARES KING MOVES TO MUST HAVE NO ATTACKERS
     *          * IF >1 PIECES PINNING KING, ONLY KING CAN MOVE
     *      - ONLY ALLOW NON-KING PIECES TO MOVE IN A WAY THAT DOESN'T GET THE KING IN CHECK
     *          * MOVE BETWEEN THE KING AND THE PIECE PINNING THE NON-KING
     *          * CAPTURE THE PINNING PIECE

     */
    if (checkers) {

        // ONLY KING CAN MOVE
        if (Count_Bits(checkers) > 1) {
            if (piece != KING) { return 0; }

            board.Remove_Piece(king_square, KING, colour);
            BB all_attacked_square = Attack_Tables::Generate_All_Attacks(enemy_colour, board);
            board.Place_Piece(king_square, KING, colour);

            return ~all_attacked_square & moves;

        }
        // if the moving piece is the king, either move away or capture
        if (piece == KING) {
            board.Remove_Piece(king_square, KING, colour);
            BB all_attacked_square = Attack_Tables::Generate_All_Attacks(enemy_colour, board);
            board.Place_Piece(king_square, KING, colour);

            return ~all_attacked_square & moves;
        }

        // EITHER CAPTURE OR MOVE ALONG THE RAY OF THE PINNING PIECE
        const int checker_square = Get_LSB(checkers);
        BB block_mask = Attack_Tables::Get_Between_Squares(king_square, checker_square);
        block_mask |= checkers;
        moves &= block_mask;
    }

    if (piece == KING) {
        const BB all_attacked_square = Attack_Tables::Generate_All_Attacks(enemy_colour, board);
        return ~all_attacked_square & moves;
    }

    BB pinned = Get_Pinned_Pieces(king_square, colour, board);

    while (pinned) {
        const int single_pin = Get_LSB(pinned);
        pinned &= pinned - 1;

        BB pin_line = Attack_Tables::Get_Between_Squares(king_square, single_pin) | (1ULL << single_pin);
        if (pin_line & (1ULL << square)) { moves &= pin_line; }
    }

    return moves;
}

BB Move_Generator::Get_Pinned_Pieces(const int king_square, const PieceColour colour, const Board& board) {

    BB pinned = 0;
    const BB friendly_pieces = colour == WHITE ? board.Get_White_Pieces() : board.Get_Black_Pieces();
    const BB enemy_pieces = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();

    BB diagonal_pinners = (board.Get_Piece(BISHOP, !colour) | board.Get_Piece(QUEEN, !colour)) &
                            Attack_Tables::Get_Bishop_Moves(king_square, 0ULL);
    BB orthogonal_pinners = (board.Get_Piece(ROOK, !colour) | board.Get_Piece(QUEEN, !colour)) &
                            Attack_Tables::Get_Rook_Moves(king_square, 0ULL);

    while (diagonal_pinners) {
        const int square = Get_LSB(diagonal_pinners);
        BB between_squares = Attack_Tables::Get_Between_Squares(king_square, square);
        BB potential_pinned = between_squares & friendly_pieces;

        if (Count_Bits(potential_pinned) == 1 && !(between_squares & enemy_pieces)) {
            pinned |= 1ULL << square;
        }
        diagonal_pinners &= diagonal_pinners - 1;
    }

    while (orthogonal_pinners) {
        const int square = Get_LSB(orthogonal_pinners);
        BB between_squares = Attack_Tables::Get_Between_Squares(king_square, square);
        BB potential_pinned = between_squares & friendly_pieces;

        if (Count_Bits(potential_pinned) == 1 && !(between_squares & enemy_pieces)) {
            pinned |= 1ULL << square;
        }
        orthogonal_pinners &= orthogonal_pinners - 1;
    }
    return pinned;
}

BB Move_Generator::Get_Pin_Line(const int pinned_square, const int king_square, const Board& board) {
    BB pin_line = Attack_Tables::Get_Between_Squares(pinned_square, king_square);
    const BB occupancy = board.Get_All_Pieces();
    pin_line |= 1ULL << pinned_square;

    const int pinned_rank = pinned_square / 8;
    const int pinned_file = pinned_square % 8;
    const int king_rank = pinned_rank / 8;
    const int king_file = pinned_rank % 8;

    if (pinned_rank == king_rank || pinned_file == king_file) {
        pin_line |= Attack_Tables::Get_Rook_Moves(pinned_square, occupancy);
    }
    else if (Same_Diagonal(pinned_square, king_square)) {
        pin_line |= Attack_Tables::Get_Bishop_Moves(pinned_square, occupancy);
    }

    return pin_line;
}


bool Move_Generator::Make_Move(Move& move, const bool generator, Board& board) {
    /*
     *  - ALL KINDS OF MOVES:
     *      * QUIET MOVE (DONE)
     *      * CAPTURE & EN_PASSANT (EN_PASSANT DONE) (CAPTURE DONE)
     *      * CASTLING (DONE)
     *      * PROMOTION (DONE)
     *
     */
    const auto piece = static_cast<PieceType>(move.Get_Piece());
    if (piece == NO_PIECE) { return false; }

    const int destination = move.Get_To();
    const int position = move.Get_From();
    const auto colour = static_cast<PieceColour>(move.Get_Colour());
    const BB enemy_pieces = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();
    const PieceColour enemy_colour = colour == WHITE ? BLACK : WHITE;

    const BB legal_moves = Get_Legal_Moves(position, piece, colour, board);
    if (!legal_moves || !(legal_moves & 1ULL << destination)) { return false; }
    board.Save_State();

    if (piece == PAWN) {
        if (1ULL << destination & (colour == BLACK ? RANK_1 : RANK_8)) {
            if (generator) {
                board.Place_Piece(destination, move.Get_Promotion_Piece(), colour);
            } else {
                const PieceType promotion_piece = Choose_Promotion_Piece();
                move.Set_Promotion_Piece(promotion_piece);
                board.Place_Piece(destination, promotion_piece, colour);
            }
            board.Remove_Piece(position, PAWN, colour);
            board.Add_Move(move);
            board.current_turn = colour == WHITE ? BLACK : WHITE;
            return true;
        }

        else if (destination == board.en_passant_square) {
            const int captured_square = destination + ((-16 * colour) + 8);
            board.Remove_Piece(captured_square, PAWN, enemy_colour);
            board.en_passant_square = -1;
            move.Set_Captured_Piece(PAWN);
            move.Set_Capture_Position(captured_square);
        }

        else if (abs(destination - position) == 16) {
            board.en_passant_square = destination + ((-16 * colour) + 8);
        }
    }

    if (1ULL << destination & enemy_pieces) {
        const PieceType captured_piece = board.Get_Piece_At_Square(destination, enemy_colour);
        board.Remove_Piece(destination, captured_piece, enemy_colour);
        move.Set_Capture_Position(destination);
        move.Set_Captured_Piece(captured_piece);

    }

    if (piece == KING) {
        if (abs(destination - position) == 2 && (1ULL << destination) & legal_moves) {
            board.Castle(move);
            board.Add_Move(move);
            board.current_turn = colour == WHITE ? BLACK : WHITE;
            return true;
        }
        if (colour == WHITE) {
            board.castling.white_king_side = false;
            board.castling.white_queen_side = false;
        }
        else {
            board.castling.black_king_side = false;
            board.castling.black_queen_side = false;
        }
    }

    if (piece == ROOK) {
        const BB position_bb = 1ULL << position;
        if (position_bb & Board::WHITE_QUEEN_ROOK) { board.castling.white_queen_side = false; }
        if (position_bb & Board::WHITE_KING_ROOK) { board.castling.white_king_side = false; }
        if (position_bb & Board::BLACK_QUEEN_ROOK) { board.castling.black_queen_side = false; }
        if (position_bb & Board::BLACK_KING_ROOK) { board.castling.black_king_side = false; }
    }


    board.Move_Piece(position, destination, piece, colour);
    board.Add_Move(move);
    board.current_turn = colour == WHITE ? BLACK : WHITE;
    return true;
}


PieceType Move_Generator::Choose_Promotion_Piece() {
    std::cout << "Choose promotion piece: " << std::endl;
    std::cout << "1. Knight\n" << "2. Bishop\n" << "3. Rook\n" << "4. Queen" << std::endl;
    int choice;

    std::cin >> choice;
    switch (choice) {
        case 1:
            return KNIGHT;
        case 2:
            return BISHOP;
        case 3:
            return ROOK;
        case 4:
            return QUEEN;
        default: {
            std::cin.clear();
            std::cerr << "Choose promotion piece: " << std::endl;
            std::cout << "1. Knight\n" << "2. Bishop\n" << "3. Rook\n" << "4. Queen" << std::endl;
            std::cin >> choice;
        }
    }
    return NO_PIECE;
}


std::vector<Move> Move_Generator::Generate_All_Moves(Board board) {
    std::vector<Move> moves;
    const PieceColour colour = board.current_turn;

    for (const auto& piece : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
        BB piece_bb = board.Get_Piece(piece, colour);

        while (piece_bb) {
            const int piece_square = Get_LSB(piece_bb);
            piece_bb &= piece_bb - 1;

            BB legal_moves = Get_Legal_Moves(piece_square, piece, colour, board);

            while (legal_moves) {
                const int legal_move_square = Get_LSB(legal_moves);
                legal_moves &= legal_moves - 1;
                Move move = Move(piece_square, legal_move_square, piece, colour);

                if (piece == PAWN) {
                    if (1ULL << legal_move_square & (colour == BLACK ? RANK_1 : RANK_8)) {
                        for (const PieceType promotion_pieces : {KNIGHT, BISHOP, ROOK, QUEEN}) {
                            Move temp_move = move;
                            temp_move.Set_Promotion_Piece(promotion_pieces);
                            if (Make_Move(temp_move, true, board)) {
                                moves.push_back(temp_move);
                                board.Undo_Move();
                            }
                        }
                        continue;
                    }
                }

                if (Make_Move(move, true, board)) {
                    moves.push_back(move);
                    board.Undo_Move();
                }
            }

        }
    }
    return moves;
}


void Move_Generator::Display_All_Moves(const std::vector<Move>& moves) {
    if(moves.empty()) return;

    int current_type = moves[0].Get_Piece();
    int current_colour = moves[0].Get_Colour();
    std::cout << Get_Piece_Name(current_type + (6 * current_colour)) << ":\n";

    for(const Move& move : moves) {
        if(current_type != move.Get_Piece()) {
            current_type = move.Get_Piece();
            std::cout << "\n" << Get_Piece_Name(current_type) << ":\n";
        }

        std::cout << Square_To_String(move.Get_From())
                 << Square_To_String(move.Get_To());

        if(move.Get_Captured_Piece() != NO_PIECE) std::cout << " (CAPTURE)";
        if(move.Is_Castling()) std::cout << " (CASTLE)";
        if(move.Get_En_Passant() != 0) std::cout << " (EN PASSANT)";
        if(move.Get_Promotion_Piece() != NO_PIECE) {
            std::cout << " (PROMOTE to " << Get_Piece_Name(move.Get_Promotion_Piece()) << ")";
        }

        std::cout << " | ";
    }
    std::cout << std::endl;
}



