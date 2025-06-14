#pragma once

#include <array>
#include "Declarations.h"
#include "move.h"


class Board {

    union {
        struct {
            BB white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king;
            BB black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king;
        };
        std::array<BB, 12> pieces;
    };

    BB white_pieces, black_pieces;



    static constexpr BB WHITE_KING_CASTLE = 0x5000000000000000ULL;
    static constexpr BB WHITE_QUEEN_CASTLE = 0x1400000000000000ULL;

    static constexpr BB BLACK_KING_CASTLE = 0x50ULL;
    static constexpr BB BLACK_QUEEN_CASTLE = 0x14ULL;


    union CastlingRights {
        struct {
            bool white_king_side : 1;
            bool white_queen_side : 1;
            bool black_king_side : 1;
            bool black_queen_side : 1;
        };
        uint8_t rights;
        [[nodiscard]] bool none() const { return rights == 0; }
    };


    struct GameState {
        CastlingRights castling_rights;
        int en_passant_square;
    };

    size_t move_count = 0;
    size_t state_count = 0;
    static constexpr int MAX_GAME_LENGTH = 300;
    int half_clock = 0;

    public:

    static constexpr BB WHITE_KING_PATH = 0x6000000000000000ULL;
    static constexpr BB BLACK_KING_PATH = 0x60ULL;
    static constexpr BB WHITE_QUEEN_PATH = 0xe00000000000000ULL;
    static constexpr BB BLACK_QUEEN_PATH = 0xeULL;
    static constexpr BB WHITE_QUEEN_ROOK = 0x900000000000000ULL;
    static constexpr BB BLACK_KING_ROOK = 0xa0ULL;
    static constexpr BB WHITE_KING_ROOK = 0xa000000000000000ULL;
    static constexpr BB BLACK_QUEEN_ROOK = 0x9ULL;

    static constexpr BB WHITE_KING_SHORT = 0x6000000000000000ULL;
    static constexpr BB BLACK_KING_SHORT = 0x60ULL;
    static constexpr BB BLACK_KING_LONG = 0xcULL;
    static constexpr BB WHITE_KING_LONG = 0xc00000000000000ULL;
    PieceColour current_turn = WHITE;

    CastlingRights castling = {false, false, false, false};

    int en_passant_square = -1;

    std::array<Move, MAX_GAME_LENGTH> move_history;
    std::array<GameState, MAX_GAME_LENGTH> state_history;

    void Add_Move(const Move& move) {
        move_history[move_count++] = move;

    }
    void Save_State() {
        state_history[state_count++] = {
        .castling_rights = castling,
        .en_passant_square = en_passant_square};
    }

    void Undo_Move() {
        if (move_count == 0) { return; }

        Move& last_move = move_history[--move_count];
        const GameState& prev = state_history[--state_count];

        castling = prev.castling_rights;
        en_passant_square = prev.en_passant_square;

        const int colour = last_move.Get_Colour();
        const int position = last_move.Get_To();

        if (last_move.Get_Captured_Piece() != NO_PIECE) {
            Place_Piece(last_move.Get_Capture_Position(), last_move.Get_Captured_Piece(), !colour);
        }
        if (last_move.Get_Castle_Side() != NO_CASTLE) {
            Castle(last_move, true);
            current_turn = current_turn == WHITE ? BLACK : WHITE;
            return;
        }

        if (last_move.Get_Promotion_Piece() != NO_PIECE) {
            Remove_Piece(position, last_move.Get_Promotion_Piece(), colour);
            Place_Piece(last_move.Get_From(), last_move.Get_Piece(), colour);
            current_turn = current_turn == WHITE ? BLACK : WHITE;
            return;
        }

        Move_Piece(position, last_move.Get_From(), last_move.Get_Piece(), colour);
        current_turn = current_turn == WHITE ? BLACK : WHITE;

    }

    [[nodiscard]] constexpr BB Get_Piece(const int piece, const int colour) const {
        return pieces[piece + (6 * colour)];
    }

    [[nodiscard]] constexpr BB Get_White_Pieces() const { return white_pieces; }
    [[nodiscard]] constexpr BB Get_Black_Pieces() const { return black_pieces; }
    [[nodiscard]] constexpr BB Get_All_Pieces() const { return white_pieces | black_pieces; }

     void constexpr Move_Piece(const int from, const int to, const int piece, const int colour) {
        const BB move_mask = 1ULL << from | 1ULL << to;
        pieces[piece + (6 * colour)] ^= move_mask;

        if (colour == WHITE) {
            white_pieces ^= move_mask;
        } else {
            black_pieces ^= move_mask;
        }
    }

    void constexpr Remove_Piece(const int square, const int piece, const int colour) {
        const BB move_mask = ~(1ULL << square);
        pieces[piece + (colour == BLACK ? 6 : 0)] &= move_mask;

        if (colour == WHITE) {
            white_pieces &= move_mask;
        } else {
            black_pieces &= move_mask;
        }
    }

    void constexpr Place_Piece(const int square, const int piece, const int colour) {
        const BB move_mask = 1ULL << square;
        pieces[piece + (6 * colour)] |= move_mask;

        if (colour == WHITE) {
            white_pieces ^= move_mask;
        } else {
            black_pieces ^= move_mask;
        }
    }

    [[nodiscard]] constexpr PieceType Get_Piece_At_Square(const int square, const int colour) const {
        const BB move_mask = 1ULL << square;
        const int index = 6 * colour;

        for (int i =0 ; i < 6; i++) {
            if (pieces[index + i] & move_mask) {
                return static_cast<PieceType>(i);
            }
        }
        return NO_PIECE;
    }


    void Castle(Move& move, const bool reverse=false) {
        const int colour = move.Get_Colour();
        const CastleSide castle_side = move.Get_To() > move.Get_From() ? KING_SIDE : QUEEN_SIDE;
        if (!reverse){ move.Set_Castle_Side(castle_side); }


        switch (colour) {
            case WHITE: {
                castle_side == KING_SIDE ? Castle_Impl<WHITE, KING_SIDE>(reverse) :
                                           Castle_Impl<WHITE, QUEEN_SIDE>(reverse);
                break;
            }

            case BLACK:
                castle_side == KING_SIDE ? Castle_Impl<BLACK, KING_SIDE>(reverse) :
                                           Castle_Impl<BLACK, QUEEN_SIDE>(reverse);
                break;

            default:;
        }

    }

    template<PieceColour colour, CastleSide side>
    void constexpr Castle_Impl(const bool reverse=false) {
        if constexpr (colour == WHITE) {
            if constexpr (side == KING_SIDE) {
                pieces[KING] ^= WHITE_KING_CASTLE;
                pieces[ROOK] ^= WHITE_KING_ROOK;
                white_pieces ^= (WHITE_KING_CASTLE | WHITE_KING_ROOK);

            } else {
                pieces[KING] ^= WHITE_QUEEN_CASTLE;
                pieces[ROOK] ^= WHITE_QUEEN_ROOK;
                white_pieces ^= (WHITE_QUEEN_CASTLE | WHITE_QUEEN_ROOK);

            }
            if constexpr (side == KING_SIDE) {
                castling.white_king_side = reverse;
            } else {
                castling.white_queen_side = reverse;
            }
            if (!reverse) {
                castling.white_king_side = false;
                castling.white_queen_side = false;
            }

        } else {
            if constexpr (side == KING_SIDE) {
                pieces[KING + 6] ^= BLACK_KING_CASTLE;
                pieces[ROOK + 6] ^= BLACK_KING_ROOK;
                black_pieces ^= (BLACK_KING_CASTLE | BLACK_KING_ROOK);

            } else {
                pieces[KING + 6] ^= BLACK_QUEEN_CASTLE;
                pieces[ROOK + 6] ^= BLACK_QUEEN_ROOK;
                black_pieces ^= (BLACK_QUEEN_CASTLE | BLACK_QUEEN_ROOK);

            }
            if constexpr (side == KING_SIDE) {
                castling.black_king_side = reverse;
            } else {
                castling.black_queen_side = reverse;
            }
            // Always disable all castling rights after castling
            if (!reverse) {
                castling.black_king_side = false;
                castling.black_queen_side = false;
            }
        }

    }

    [[nodiscard]] constexpr BB Get_Castle_Moves(const PieceColour colour, const BB enemy_attacks) const {
        BB castle_moves = 0ULL;
        const BB occupancy = Get_All_Pieces();
        const BB king_bb = pieces[KING + (colour * 6)];
        if (colour == WHITE) {
            const bool king_side_valid =
                !(WHITE_KING_PATH & occupancy) &&
                (WHITE_KING_ROOK & pieces[ROOK]) &&
                !(enemy_attacks & (WHITE_KING_SHORT | king_bb)) &&
                castling.white_king_side;
            castle_moves |= (king_side_valid * (1ULL << g1));

            const bool queen_side_valid =
                !(WHITE_QUEEN_PATH & occupancy) &&
                (WHITE_QUEEN_ROOK & pieces[ROOK]) &&
                !(enemy_attacks & (king_bb | WHITE_KING_LONG)) &&
                castling.white_queen_side;
            castle_moves |= (queen_side_valid * (1ULL << c1));
        } else {
            const bool king_side_valid =
                !(BLACK_KING_PATH & occupancy) &&
                (BLACK_KING_ROOK & pieces[ROOK + 6]) &&
                !(enemy_attacks & (king_bb | BLACK_KING_SHORT)) &&
                castling.black_king_side;
            castle_moves |= (king_side_valid * (1ULL << g8));

            const bool queen_side_valid =
                !(BLACK_QUEEN_PATH & occupancy) &&
                (BLACK_QUEEN_ROOK & pieces[ROOK + 6]) &&
                !(enemy_attacks & (king_bb | BLACK_KING_LONG)) &&
                castling.black_queen_side;
            castle_moves |= (queen_side_valid * (1ULL << c8));
        }

        return castle_moves;
    }

    void Initialise_From_Fen(const std::string& fen) {

        pieces.fill(0);
        white_pieces = 0;
        black_pieces = 0;
        en_passant_square = -1;

        int square = 0;
        size_t pos = 0;

        while (fen[pos] != ' ') {
            if (const char c = fen[pos++]; c == '/') {
                continue;
            } else if (c >= '1' && c <= '8') {
                square += (c - '0');
            } else {
                const BB square_bb = 1ULL << square;
                switch (c) {
                    case 'P': pieces[0] |= square_bb; break;
                    case 'N': pieces[1] |= square_bb; break;
                    case 'B': pieces[2] |= square_bb; break;
                    case 'R': pieces[3] |= square_bb; break;
                    case 'Q': pieces[4] |= square_bb; break;
                    case 'K': pieces[5] |= square_bb; break;
                    case 'p': pieces[6] |= square_bb; break;
                    case 'n': pieces[7] |= square_bb; break;
                    case 'b': pieces[8] |= square_bb; break;
                    case 'r': pieces[9] |= square_bb; break;
                    case 'q': pieces[10] |= square_bb; break;
                    case 'k': pieces[11] |= square_bb; break;

                    default:
                        break;
                }
                square++;
            }
        }
        pos++;
        current_turn = (fen[pos] == 'w') ? WHITE : BLACK;
        pos += 2;

        while (fen[pos] != ' ') {
            switch (fen[pos++]) {
                case 'K':
                    castling.white_king_side = true;
                    break;
                case 'Q':
                    castling.white_queen_side = true;
                    break;
                case 'k':
                    castling.black_king_side = true;
                    break;
                case 'q':
                    castling.black_queen_side = true;
                    break;

                default:;
            }
        }
        pos++;
        if (fen[pos] != '-') {
            char file = fen[pos];
            char rank = fen[pos + 1];
            en_passant_square = (8 - (rank - '0')) * 8 + (file - 'a');
        }

        white_pieces = pieces[0] | pieces[1] | pieces[2] |
                       pieces[3] | pieces[4] | pieces[5];

        black_pieces = pieces[6] | pieces[7] | pieces[8] |
                       pieces[9] | pieces[10] | pieces[11];
    }


    void Print_Detailed_Board() const {
        int count = 8;
        for(int file = 0; file < 8; file++) {
            std::cout << count << "  ";

            for(int rank = 0; rank < 8; rank++) {
                int position = file * 8 + rank;
                char pieceChar = '.';

                if(Get_Bit(pieces[0], position)) pieceChar = 'P';
                else if(Get_Bit(pieces[1], position)) pieceChar = 'N';
                else if(Get_Bit(pieces[2], position)) pieceChar = 'B';
                else if(Get_Bit(pieces[3], position)) pieceChar = 'R';
                else if(Get_Bit(pieces[4], position)) pieceChar = 'Q';
                else if(Get_Bit(pieces[5], position)) pieceChar = 'K';

                else if(Get_Bit(pieces[6], position)) pieceChar = 'p';
                else if(Get_Bit(pieces[7], position)) pieceChar = 'n';
                else if(Get_Bit(pieces[8], position)) pieceChar = 'b';
                else if(Get_Bit(pieces[9], position)) pieceChar = 'r';
                else if(Get_Bit(pieces[10], position)) pieceChar = 'q';
                else if(Get_Bit(pieces[11], position)) pieceChar = 'k';

                std::cout << pieceChar << " ";
            }
            std::cout << std::endl;
            count -= 1;
        }
        std::cout << "   a b c d e f g h" << std::endl;
        std::cout << std::endl;
    }

    [[nodiscard]] std::string Board_To_Fen() const {
    std::string fen;
    int emptyCount = 0;

    // Loop through board ranks (8->1)
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            char pieceChar = 0;  // Use 0 instead of '.' for empty squares

            // Check each piece type/color
            if (Get_Bit(pieces[0], square)) pieceChar = 'P';
            else if (Get_Bit(pieces[1], square)) pieceChar = 'N';
            else if (Get_Bit(pieces[2], square)) pieceChar = 'B';
            else if (Get_Bit(pieces[3], square)) pieceChar = 'R';
            else if (Get_Bit(pieces[4], square)) pieceChar = 'Q';
            else if (Get_Bit(pieces[5], square)) pieceChar = 'K';
            else if (Get_Bit(pieces[6], square)) pieceChar = 'p';
            else if (Get_Bit(pieces[7], square)) pieceChar = 'n';
            else if (Get_Bit(pieces[8], square)) pieceChar = 'b';
            else if (Get_Bit(pieces[9], square)) pieceChar = 'r';
            else if (Get_Bit(pieces[10], square)) pieceChar = 'q';
            else if (Get_Bit(pieces[11], square)) pieceChar = 'k';

            if (pieceChar == 0) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                fen += pieceChar;
            }
        }

        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
            emptyCount = 0;
        }

        if (rank < 7) {
            fen += '/';
        }
    }

    // Add turn
    fen += current_turn == WHITE ? " w " : " b ";

    std::string castling_rights;
    if (castling.white_king_side) castling_rights += 'K';
    if (castling.white_queen_side) castling_rights += 'Q';
    if (castling.black_king_side) castling_rights += 'k';
    if (castling.black_queen_side) castling_rights += 'q';

    fen += castling.none() ? "-" : castling_rights;

        fen += " ";
        if (en_passant_square != -1) {
            // Convert square index to algebraic notation (e.g., e3)
            char file = 'a' + (en_passant_square % 8);
            char rank = '8' - (en_passant_square / 8);
            fen += file;
            fen += rank;
        } else {
            fen += "-";
        }

        fen += " " + std::to_string(half_clock) + " 1";

    return fen;
}


};