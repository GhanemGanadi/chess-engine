#include "Move_Generator.h"
#include "Attack_Tables.h"

BB Move_Generator::Get_Pseudo_Legal_Moves(const int square, const PieceType piece, const PieceColour colour,
                                                    const Board &board) {

    const BB friendly = colour == WHITE ? board.Get_White_Pieces() : board.Get_Black_Pieces();
    const BB enemy = colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();
    const BB occupancy = friendly | enemy;

    switch (piece) {
        case PAWN:
            return Attack_Tables::Get_Pawn_Moves(square, colour, friendly) |
                    Attack_Tables::Get_Pawn_Attacks(square, colour, enemy, board.en_passant_square);

        case KNIGHT:
            return Attack_Tables::Get_Knight_Moves(square, friendly);

        case BISHOP:
            return Attack_Tables::Get_Bishop_Moves(square, occupancy);

        case ROOK:
            return Attack_Tables::Get_Rook_Moves(square, occupancy);

        case QUEEN:
            return Attack_Tables::Get_Queen_Moves(square, occupancy);

        case KING:
            return Attack_Tables::Get_King_Moves(square, friendly);

        default:;
    }
    return 0ULL;
}


BB Move_Generator::Get_Legal_Moves(const int square, const PieceType piece, const PieceColour colour,
                                    const Board &board) {

    BB moves = Get_Pseudo_Legal_Moves(square, piece, colour, board);
    const int king_square =  Get_LSB(board.Get_Piece(KING, colour));

    BB checkers = Attack_Tables::Get_Attacks_From_Square(king_square, colour, board);

    /*
     *  WHAT MUST THE FUNCTION DO;
     *      - ONLY ALLOW THE KING TO MOVE TO A SAFE SQUARE
     *          * ALL SQUARES KING MOVES TO MUST HAVE NO ATTACKERS
     *          * IF >1 PIECES PINNING KING, ONLY KING CAN MOVE
     *      - ONLY ALLOW NON-KING PIECES TO MOVE IN A WAY THAT DOESNT GET THE KING IN CHECK
     *          * MOVE BETWEEN THE KING AND THE PIECE PINNING THE NON-KING
     *          * CAPTURE THE PINNING PIECE
     *
     *      -
     *
     *
     */
    if (checkers) {

        // ONLY KING CAN MOVE
        if (Count_Bits(checkers) > 1) {
            if (piece != KING) { return 0; }
            BB safe_moves = 0ULL;

            while (moves) {
                const int destination = Get_LSB(moves);
                if (Attack_Tables::Get_Attacks_From_Square(destination, colour, board) == 0ULL) {
                    safe_moves |= moves;
                }
                moves &= moves - 1;
            }
            return safe_moves;
        }
        // if the moving piece is the king, either move away or capture
        if (piece == KING) {
            BB safe_moves = 0ULL;
            while (moves) {
                const int destination = Get_LSB(moves);
                if (Attack_Tables::Get_Attacks_From_Square(destination, colour, board) == 0ULL) {
                    safe_moves |= moves;
                }
                moves &= moves - 1;
            }
            return safe_moves;
        }

        // EITHER CAPTURE OR MOVE ALONG THE RAY OF THE PINNING PIECE
        const int checker_square = Get_LSB(checkers);
        BB block_mask = Attack_Tables::Get_Between_Squares(square, checker_square);
        block_mask |= checkers;
        moves &= block_mask;
    }

    if (piece == KING) {
        BB safe_moves = 0ULL;
        while (moves) {
            const int destination = Get_LSB(moves);
            if (Attack_Tables::Get_Attacks_From_Square(destination, colour, board) == 0ULL) {
                safe_moves |= moves;
            }
            moves &= moves - 1;
        }
        return safe_moves;
    }

    const BB pinned = Get_Pinned_Pieces(king_square, colour, board);
    if (pinned & (1ULL << square)) {
        BB pin_line = Get_Pin_Line(square, king_square, board);
        moves &= pin_line;
    }

    return moves;
}

BB Move_Generator::Get_Pinned_Pieces(const int king_square, const PieceColour colour, const Board& board) {

    BB pinned = 0;
    const BB friendly_pieces = colour == WHITE ? board.Get_White_Pieces() : board.Get_Black_Pieces();
    const PieceColour enemy_colour = colour == WHITE ? BLACK : WHITE;

    BB potential_pinners = board.Get_Piece(BISHOP, enemy_colour) | board.Get_Piece(ROOK, enemy_colour) |
                            board.Get_Piece(QUEEN, enemy_colour);

    while (potential_pinners) {
        int pinner_square = Get_LSB(potential_pinners);
        BB between_squares = Attack_Tables::Get_Between_Squares(king_square, pinner_square);
        if (Count_Bits(between_squares & friendly_pieces)) {
            pinned |= (between_squares & potential_pinners);
        }
        potential_pinners &= potential_pinners - 1;
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

