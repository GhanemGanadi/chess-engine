#include "Tables.h"

#include "../Board/Board.h"


std::vector<U64> Tables::Rook_Magic_Numbers;
std::vector<U64> Tables::Bishop_Magic_Numbers;

U64 Tables::Bishop_Moves[64][512];
    U64 Tables::Rook_Moves[64][4096];
    U64 Tables::Bishop_Masks[64];
    U64 Tables::Rook_Masks[64];

    int Tables::Relevant_Rook_Squares[64] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12};



    int Tables::Bishop_Relevant_Squares[64] ={
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6};

U64 Tables::Generate_Rook_Attacks(const int square) {
    U64 attacks = 0ULL;
    const int targetRank = square / 8;
    const int targetFile = square % 8;

    for(int rank = targetRank + 1; rank <= 6; rank++) {attacks |= (1ULL << (rank * 8 + targetFile));}
    for(int rank = targetRank - 1; rank >= 1; rank--) {attacks |= (1ULL << (rank * 8 + targetFile));}
    for(int file = targetFile + 1; file <= 6; file++) {attacks |= (1ULL << (targetRank * 8 + file));}
    for(int file = targetFile - 1; file >= 1; file--) {attacks |= (1ULL << (targetRank * 8 + file));}

    return attacks;
}

U64 Tables::Generate_Rook_Attacks_With_Blocks(const int square, const U64 block) {
    U64 attacks = 0ULL;
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

U64 Tables::Generate_Bishop_Attacks(const int square) {
    U64 attacks = 0ULL;
    const int targetRank = square / 8;
    const int targetFile = square % 8;
    int rank, file;

    for(rank = targetRank + 1, file = targetFile + 1; rank <= 6 && file <= 6; rank++, file++) {attacks |= (1ULL << (rank * 8 + file));}
    for(rank = targetRank - 1, file = targetFile + 1; rank >= 1 && file <= 6; rank--, file++) {attacks |= (1ULL << (rank * 8 + file));}
    for(rank = targetRank + 1, file = targetFile - 1; rank <= 6 && file >= 1; rank++, file--) {attacks |= (1ULL << (rank * 8 + file));}
    for(rank = targetRank - 1, file = targetFile - 1; rank >= 1 && file >= 1; rank--, file--) {attacks |= (1ULL << (rank * 8 + file));}

    return attacks;
}

U64 Tables::Generate_Bishop_Attacks_With_Blocks(const int square, const U64 block) {
    U64 attacks = 0ULL;
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

void Tables::Initialise_Attacks() {

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

    // Initialize masks
    for(int square = 0; square < 64; square++) {
        Bishop_Masks[square] = Generate_Bishop_Attacks(square);
        Rook_Masks[square] = Generate_Rook_Attacks(square);
    }

    // Initialize move tables
    for(int square = 0; square < 64; square++) {
        const int bishopBits = Bishop_Relevant_Squares[square];
        const int rookBits = Relevant_Rook_Squares[square];

        const int rookVariation = 1 << rookBits;
        const int bishopVariation = 1 << bishopBits;

        for(int index = 0; index < rookVariation; index++) {
            const U64 occupancy = Set_Occupancy(index, rookBits, Rook_Masks[square]);
            const int magicIndex = static_cast<int>((occupancy * Rook_Magic_Numbers[square]) >> (64 - rookBits));
            Rook_Moves[square][magicIndex] = Generate_Rook_Attacks_With_Blocks(square, occupancy);
        }

        for(int index = 0; index < bishopVariation; index++) {
            const U64 occupancy = Set_Occupancy(index, bishopBits, Bishop_Masks[square]);
            const int magicIndex = static_cast<int>((occupancy * Bishop_Magic_Numbers[square])
                                >> (64 - bishopBits));
            Bishop_Moves[square][magicIndex] = Generate_Bishop_Attacks_With_Blocks(square, occupancy);
        }
    }
}
U64 Tables::Get_Bishop_Moves(const int square, U64 occupancy) {
    occupancy &= Bishop_Masks[square];
    occupancy *= Bishop_Magic_Numbers[square];
    occupancy >>= 64 - Bishop_Relevant_Squares[square];
    return Bishop_Moves[square][occupancy];
}

U64 Tables::Get_Rook_Moves(const int square, U64 occupancy) {
    occupancy &= Rook_Masks[square];
    occupancy *= Rook_Magic_Numbers[square];
    occupancy >>= 64 - Relevant_Rook_Squares[square];
    return Rook_Moves[square][occupancy];
}

U64 Tables::Get_Queen_Moves(const int square, const U64 occupancy) {
    return Get_Bishop_Moves(square, occupancy) | Get_Rook_Moves(square, occupancy);
}
