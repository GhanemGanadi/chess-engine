#include "Attack_Tables.h"

std::once_flag Attack_Tables::flag;

std::array<BB, 64 * 4096> Attack_Tables::Rook_Magic_Numbers;
std::array<BB, 64 * 512>  Attack_Tables::Bishop_Magic_Numbers;


std::array<BB, 64> Attack_Tables::Knight_Moves;
std::array<BB, 64> Attack_Tables::King_Moves;
std::array<std::array<BB, 64>, 2> Attack_Tables::Pawn_Attacks;
std::array<std::array<BB, 64>, 2> Attack_Tables::Pawn_Moves;


BB Attack_Tables::Bishop_Moves[64][512];
BB Attack_Tables::Rook_Moves[64][4096];
BB Attack_Tables::Bishop_Masks[64];
BB Attack_Tables::Rook_Masks[64];
BB Attack_Tables::Between_Squares[64][64];

int Attack_Tables::Relevant_Rook_Squares[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12};



int Attack_Tables::Bishop_Relevant_Squares[64] ={
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6};

void Attack_Tables::Ensure_Initialised() {
    std::call_once(flag, []() {
        Initialise_Attacks();
    });
}

BB Attack_Tables::Generate_Rook_Attacks(const int square) {
    BB attacks = 0ULL;
    const int targetRank = square / 8;
    const int targetFile = square % 8;

    for(int rank = targetRank + 1; rank <= 6; rank++) {attacks |= (1ULL << (rank * 8 + targetFile));}
    for(int rank = targetRank - 1; rank >= 1; rank--) {attacks |= (1ULL << (rank * 8 + targetFile));}
    for(int file = targetFile + 1; file <= 6; file++) {attacks |= (1ULL << (targetRank * 8 + file));}
    for(int file = targetFile - 1; file >= 1; file--) {attacks |= (1ULL << (targetRank * 8 + file));}

    return attacks;
}

BB Attack_Tables::Generate_Rook_Attacks_With_Blocks(const int square, const BB block) {
    BB attacks = 0ULL;
    const int targetRank = square / 8;
    const int targetFile = square % 8;

    // North
    for(int rank = targetRank + 1; rank <= 7; rank++) {
        attacks |= (1ULL << (rank * 8 + targetFile));
        if((1ULL << (rank * 8 + targetFile)) & block) break;
    }

    // South
    for(int rank = targetRank - 1; rank >= 0; rank--) {
        attacks |= (1ULL << (rank * 8 + targetFile));
        if((1ULL << (rank * 8 + targetFile)) & block) break;
    }

    // East
    for(int file = targetFile + 1; file <= 7; file++) {
        attacks |= (1ULL << (targetRank * 8 + file));
        if((1ULL << (targetRank * 8 + file)) & block) break;
    }

    // West
    for(int file = targetFile - 1; file >= 0; file--) {
        attacks |= (1ULL << (targetRank * 8 + file));
        if((1ULL << (targetRank * 8 + file)) & block) break;
    }
    return attacks;
}

BB Attack_Tables::Generate_Bishop_Attacks(const int square) {
    BB attacks = 0ULL;
    const int targetRank = square / 8;
    const int targetFile = square % 8;
    int rank, file;

    for(rank = targetRank + 1, file = targetFile + 1; rank <= 6 && file <= 6; rank++, file++) {attacks |= (1ULL << (rank * 8 + file));}
    for(rank = targetRank - 1, file = targetFile + 1; rank >= 1 && file <= 6; rank--, file++) {attacks |= (1ULL << (rank * 8 + file));}
    for(rank = targetRank + 1, file = targetFile - 1; rank <= 6 && file >= 1; rank++, file--) {attacks |= (1ULL << (rank * 8 + file));}
    for(rank = targetRank - 1, file = targetFile - 1; rank >= 1 && file >= 1; rank--, file--) {attacks |= (1ULL << (rank * 8 + file));}

    return attacks;
}

BB Attack_Tables::Generate_Bishop_Attacks_With_Blocks(const int square, const BB block) {
    BB attacks = 0ULL;
    const int targetRank = square / 8;
    const int targetFile = square % 8;
    int rank, file;

    for(rank = targetRank + 1, file = targetFile + 1; rank <= 7 && file <= 7; rank++, file++) {
        attacks |= (1ULL << (rank * 8 + file));
        if(Get_Bit(block, rank * 8 + file)){break;}
    }

    for(rank = targetRank - 1, file = targetFile + 1; rank >= 0 && file <= 7; rank--, file++) {
        attacks |= (1ULL << (rank * 8 + file));
        if(Get_Bit(block, rank * 8 + file)){break;}
    }

    for(rank = targetRank + 1, file = targetFile - 1; rank <= 7 && file >= 0; rank++, file--) {
        attacks |= (1ULL << (rank * 8 + file));
        if(Get_Bit(block, rank * 8 + file)){break;}
    }
    for(rank = targetRank - 1, file = targetFile - 1; rank >= 0 && file >= 0; rank--, file--) {
        attacks |= (1ULL << (rank * 8 + file));
        if(Get_Bit(block, rank * 8 + file)){break;}
    }

    return attacks;
}

void Attack_Tables::Initialise_Attacks() {

    Rook_Magic_Numbers = {0x8a80104000800020ULL,0x140002000100040ULL,0x2801880a0017001ULL,0x100081001000420ULL,
    0x200020010080420ULL,0x3001c0002010008ULL,0x8480008002000100ULL,0x2080088004402900ULL,0x800098204000ULL,0x2024401000200040ULL,
    0x100802000801000ULL,0x120800800801000ULL,0x208808088000400ULL,0x2802200800400ULL,0x2200800100020080ULL,0x801000060821100ULL,
    0x80044006422000ULL,0x100808020004000ULL,0x12108a0010204200ULL,0x140848010000802ULL,0x481828014002800ULL,0x8094004002004100ULL,
    0x4010040010010802ULL,0x20008806104ULL,0x100400080208000ULL,0x2040002120081000ULL,0x21200680100081ULL,0x20100080080080ULL,
    0x2000a00200410ULL,0x20080800400ULL,0x80088400100102ULL,0x80004600042881ULL,0x4040008040800020ULL,0x440003000200801ULL,
    0x4200011004500ULL,0x188020010100100ULL,0x14800401802800ULL,0x2080040080800200ULL,0x124080204001001ULL,0x200046502000484ULL,
    0x480400080088020ULL,0x1000422010034000ULL,0x30200100110040ULL,0x100021010009ULL,0x2002080100110004ULL,0x202008004008002ULL,
    0x20020004010100ULL,0x2048440040820001ULL,0x101002200408200ULL,0x40802000401080ULL,0x4008142004410100ULL,0x2060820c0120200ULL,
    0x1001004080100ULL,0x20c020080040080ULL,0x2935610830022400ULL,0x44440041009200ULL,0x280001040802101ULL,0x2100190040002085ULL,
    0x80c0084100102001ULL,0x4024081001000421ULL,0x20030a0244872ULL,0x12001008414402ULL,0x2006104900a0804ULL,0x1004081002402ULL};

    Bishop_Magic_Numbers = {0x40040822862081ULL,0x40810a4108000ULL,0x2008008400920040ULL,0x61050104000008ULL,
    0x8282021010016100ULL,0x41008210400a0001ULL,0x3004202104050c0ULL,0x22010108410402ULL,0x60400862888605ULL,0x6311401040228ULL,
    0x80801082000ULL,0x802a082080240100ULL,0x1860061210016800ULL,0x401016010a810ULL,0x1000060545201005ULL,0x21000c2098280819ULL,
    0x2020004242020200ULL,0x4102100490040101ULL,0x114012208001500ULL,0x108000682004460ULL,0x7809000490401000ULL,0x420b001601052912ULL,
    0x408c8206100300ULL,0x2231001041180110ULL,0x8010102008a02100ULL,0x204201004080084ULL,0x410500058008811ULL,0x480a040008010820ULL,
    0x2194082044002002ULL,0x2008a20001004200ULL,0x40908041041004ULL,0x881002200540404ULL,0x4001082002082101ULL,0x8110408880880ULL,
    0x8000404040080200ULL,0x200020082180080ULL,0x1184440400114100ULL,0xc220008020110412ULL,0x4088084040090100ULL,0x8822104100121080ULL,
    0x100111884008200aULL,0x2844040288820200ULL,0x90901088003010ULL,0x1000a218000400ULL,0x1102010420204ULL,0x8414a3483000200ULL,
    0x6410849901420400ULL,0x201080200901040ULL,0x204880808050002ULL,0x1001008201210000ULL,0x16a6300a890040aULL,0x8049000441108600ULL,
    0x2212002060410044ULL,0x100086308020020ULL,0x484241408020421ULL,0x105084028429c085ULL,0x4282480801080cULL,0x81c098488088240ULL,
    0x1400000090480820ULL,0x4444000030208810ULL,0x1020142010820200ULL,0x2234802004018200ULL,0xc2040450820a00ULL,0x2101021090020ULL};


    for(int square = 0; square < 64; square++) {
        Bishop_Masks[square] = Generate_Bishop_Attacks(square);
        Rook_Masks[square] = Generate_Rook_Attacks(square);
    }

    for(int square = 0; square < 64; square++) {
        const int bishopBits = Bishop_Relevant_Squares[square];
        const int rookBits = Relevant_Rook_Squares[square];

        const int rookVariation = 1 << rookBits;
        const int bishopVariation = 1 << bishopBits;

        for(int index = 0; index < rookVariation; index++) {
            const BB occupancy = Set_Occupancy(index, rookBits, Rook_Masks[square]);
            const int magicIndex = static_cast<int>((occupancy * Rook_Magic_Numbers[square]) >> (64 - rookBits));
            Rook_Moves[square][magicIndex] = Generate_Rook_Attacks_With_Blocks(square, occupancy);
        }

        for(int index = 0; index < bishopVariation; index++) {
            const BB occupancy = Set_Occupancy(index, bishopBits, Bishop_Masks[square]);
            const int magicIndex = static_cast<int>((occupancy * Bishop_Magic_Numbers[square])
                                >> (64 - bishopBits));
            Bishop_Moves[square][magicIndex] = Generate_Bishop_Attacks_With_Blocks(square, occupancy);
        }

        Pawn_Moves[square][WHITE] = Generate_Pawn_Moves(square, WHITE);
        Pawn_Moves[square][BLACK] = Generate_Pawn_Moves(square, BLACK);
        Pawn_Attacks[square][WHITE] = Generate_Pawn_Attacks(square, WHITE);
        Pawn_Attacks[square][BLACK] = Generate_Pawn_Attacks(square, BLACK);
        Knight_Moves[square] = Generate_Knight_Attacks(square);
        King_Moves[square] = Generate_King_Attacks(square);

        for (int square2 = 0; square2 < 64; square2++) {
            Between_Squares[square][square2] = Generate_Between_Squares(square, square2);
        }
    }

}



BB Attack_Tables::Generate_Pawn_Moves(const int square, const PieceColour colour) {
    const int direction = colour == WHITE ? -8 : 8;
    if((square + direction) > 63 || (square + direction) < 0) return 0ULL;

    BB pawn = 1ULL << (square + direction);

    const bool onStartRank = colour == WHITE ? (square >= 48 && square <= 55) :
                                                (square >= 8 && square <= 15);

    if (onStartRank) { pawn |= 1ULL << (square + (2 * direction)); }

    return pawn;
}

BB Attack_Tables::Generate_Pawn_Attacks(const int square, const PieceColour colour) {
    BB attacks = 0ULL;
    const BB pawnBB = 1ULL << square;

    if (colour == WHITE) {
        attacks |= ((pawnBB >> 9) & ~H_FILE);
        attacks |= ((pawnBB >> 7) & ~A_FILE);
    } else {
        attacks |= ((pawnBB << 7) & ~H_FILE);
        attacks |= ((pawnBB << 9) & ~A_FILE);
    }
    return attacks;
}

BB Attack_Tables::Generate_King_Attacks(const int square) {
    const BB king = 1ULL << square;
    return (king << 8) | (king >> 8) |
           (king << 1 & ~A_FILE) | (king >> 1 & ~H_FILE) |
           (king << 7 & ~H_FILE) | (king >> 7 & ~A_FILE) |
           (king << 9 & ~A_FILE) | (king >> 9 & ~H_FILE);
}

BB Attack_Tables::Generate_Knight_Attacks(const int square) {
    const BB knight = 1ULL << square;
    return (knight << 6  & ~GH_FILE) | (knight >> 6  & ~AB_FILE) |
           (knight << 10 & ~AB_FILE) | (knight >> 10 & ~GH_FILE) |
           (knight << 15 & ~H_FILE) | (knight >> 15 & ~A_FILE) |
           (knight << 17 & ~A_FILE) | (knight >> 17 & ~H_FILE);
}

BB Attack_Tables::Generate_Between_Squares(const int start_square, const int end_square) {
    BB bb = 0ULL;

    const int rank1 = start_square / 8;
    const int file1 = start_square % 8;
    const int rank2 = end_square / 8;
    const int file2 = end_square % 8;

    if (rank1 == rank2) {
        const int min_file = std::min(file1, file2);
        const int max_file = std::max(file1, file2);
        for (int f = min_file + 1; f < max_file; f++) {
            bb |= (1ULL << (rank1 * 8 + f));
        }
    }

    else if (file1 == file2) {
        const int min_rank = std::min(rank1, rank2);
        const int max_rank = std::max(rank1, rank2);
        for (int r = min_rank + 1; r < max_rank; r++) {
            bb |= (1ULL << (r * 8 + file1));
        }
    }

    else if (abs(rank1 - rank2) == abs(file1 - file2)) {
        const int rank_step = (rank2 > rank1) ? 1 : -1;
        const int file_step = (file2 > file1) ? 1 : -1;

        int r = rank1 + rank_step;
        int f = file1 + file_step;

        while (r != rank2 && f != file2) {
            bb |= (1ULL << (r * 8 + f));
            r += rank_step;
            f += file_step;
        }
    }

    return bb;
}


BB Attack_Tables::Get_Pawn_Moves(const int square, const PieceColour colour, const BB friendly) {
    Ensure_Initialised();
    return (Pawn_Moves[square][colour] & ~friendly);
}

BB Attack_Tables::Get_Pawn_Attacks(const int square, const PieceColour colour, const BB enemy,
                                    const int ep_square) {
    Ensure_Initialised();
    BB moves = Pawn_Attacks[square][colour] & enemy;
    if (ep_square != -1) {
        const int rank = square / 8;
        if ((colour == WHITE && rank == 4) || (colour == BLACK && rank == 3)) {
            if (Pawn_Attacks[square][colour] & (1ULL << ep_square)) {
                moves |= 1ULL << ep_square;
            }
        }
    }
    return moves;
}


BB Attack_Tables::Get_Knight_Moves(const int square, const BB friendly) {
    Ensure_Initialised();
    return Knight_Moves[square] & ~friendly;
}

BB Attack_Tables::Get_Bishop_Moves(const int square, BB occupancy) {
    Ensure_Initialised();
    occupancy &= Bishop_Masks[square];
    occupancy *= Bishop_Magic_Numbers[square];
    occupancy >>= 64 - Bishop_Relevant_Squares[square];
    return Bishop_Moves[square][occupancy];
}

BB Attack_Tables::Get_Rook_Moves(const int square, BB occupancy) {
    Ensure_Initialised();
    occupancy &= Rook_Masks[square];
    occupancy *= Rook_Magic_Numbers[square];
    occupancy >>= 64 - Relevant_Rook_Squares[square];
    return Rook_Moves[square][occupancy];
}

BB Attack_Tables::Get_Queen_Moves(const int square, const BB occupancy) {
    return Get_Bishop_Moves(square, occupancy) | Get_Rook_Moves(square, occupancy);
}

BB Attack_Tables::Get_King_Moves(const int square, const BB friendly) {
    Ensure_Initialised();
    return King_Moves[square] & ~friendly;
}


BB Attack_Tables::Get_Between_Squares(const int square1, const int square2) {
    Ensure_Initialised();
    return Between_Squares[square1][square2];
}

BB Attack_Tables::Get_Attacks_From_Square(const int square, const PieceColour colour, const Board& board) {
    BB attackers = 0;
    const PieceColour enemy_colour = colour == WHITE ? BLACK : WHITE;
    const BB occupancy = board.Get_All_Pieces();
    board.Print_Detailed_Board();

    BB diagonal_sliders = board.Get_Piece(BISHOP, enemy_colour) | board.Get_Piece(QUEEN, enemy_colour);
    if (diagonal_sliders) {
        BB bishop_attacks = Get_Bishop_Moves(square, occupancy);
        attackers |= bishop_attacks & diagonal_sliders;
    }

    BB orthogonal_sliders = board.Get_Piece(ROOK, enemy_colour) | board.Get_Piece(QUEEN, enemy_colour);
    if (orthogonal_sliders) {
        BB rook_attacks = Get_Rook_Moves(square, occupancy);
        attackers |= rook_attacks & orthogonal_sliders;
    }

    attackers |= Get_Knight_Moves(square, 0) & board.Get_Piece(KNIGHT, enemy_colour);

    attackers |= Get_Pawn_Attacks(square, colour, 0, -1) & board.Get_Piece(PAWN, enemy_colour);

    attackers |= Get_King_Moves(square, 0) & board.Get_Piece(KING, enemy_colour);

    return attackers;
}

BB Attack_Tables::Generate_All_Attacks(const PieceColour attacking_colour, const Board &board) {
    BB attacks = 0ULL;
    const BB occupancy = board.Get_All_Pieces();
    const BB friendly_pieces = attacking_colour == WHITE ? board.Get_White_Pieces() : board.Get_Black_Pieces();
    const BB enemy_pieces = attacking_colour == WHITE ? board.Get_Black_Pieces() : board.Get_White_Pieces();

    BB pawns = board.Get_Piece(PAWN, attacking_colour);
    while (pawns) {
        const int square = Get_LSB(pawns);
        attacks |= Get_Pawn_Attacks(square, attacking_colour, enemy_pieces, board.en_passant_square);
        pawns &= pawns - 1;
    }

    BB knights = board.Get_Piece(KNIGHT, attacking_colour);
    while (knights) {
        attacks |= Knight_Moves[Get_LSB(knights)];
        knights &= knights - 1;
    }

    BB diagonal_sliders = board.Get_Piece(BISHOP, attacking_colour) | board.Get_Piece(QUEEN, attacking_colour);
    while (diagonal_sliders) {
        attacks |= Get_Bishop_Moves(Get_LSB(diagonal_sliders), occupancy);
        diagonal_sliders &= diagonal_sliders - 1;
    }

    BB orthogonal_sliders = board.Get_Piece(ROOK, attacking_colour) | board.Get_Piece(QUEEN, attacking_colour);
    while (orthogonal_sliders) {
        attacks |= Get_Rook_Moves(Get_LSB(orthogonal_sliders), occupancy);
        orthogonal_sliders &= orthogonal_sliders - 1;
    }

    const int king_square = Get_LSB(board.Get_Piece(KING, attacking_colour));
    attacks |= King_Moves[king_square];

    return attacks & ~friendly_pieces;
}






