
#include "Pawns.h"
#include "Evaluation.h"

namespace Engine {

    U64 Pawn_Evaluation::Get_Shield_Squares(const Board& board, const int kingSquare, const PieceColour colour){
        U64 pawns = 0ULL;
		const Move& previousMove = board.moveHistory.back();

    	if (!previousMove.Is_Castling())
		if (colour == WHITE) { pawns = board.Get_White_Pawn(); }
        else { pawns = board.Get_Black_Pawn(); }

	    if (kingSquare == g1 || kingSquare == g8) {
			pawns &= (FILE_H | FILE_G | FILE_F);
	    } else if (kingSquare == b1 || kingSquare == b8) {
			pawns &= (FILE_A | FILE_B | FILE_C);
		}

        return pawns;
    }

	int Pawn_Evaluation::Evaluate_Pawn_Shield(const Board &board, const PieceColour colour) {
		int score = 0;
    	const U64 kingBB = board.Get_Piece_Bitboard(KING, colour);
		const int kingSquare = Get_Square(kingBB);

		const U64 pawnShield = Get_Shield_Squares(board, kingSquare, colour);

    	// king pawn not protecting
    	if (!(kingBB >> 8 && pawnShield)) {
    		score -= SHIELD_PENALTIES[0];
    	}

    	if (!((kingBB >> 9) | (kingBB >> 7)) && pawnShield) {
    		score -= SHIELD_PENALTIES[1];
    	}

    	return score;
	}

	int Pawn_Evaluation::Evaluate_Pawn_Chain(const Board &board, const PieceColour colour) {

    	int score = 0;
		U64 pawns = board.Get_Piece_Bitboard(PAWN, colour);

    	for (size_t i = 0; i < FILES.size() - 1; i++) {
    		U64 pawnInFile = FILES[i] & pawns;

    		if ((pawnInFile >> 7 | pawnInFile << 9) & (pawns & FILES[i+1])){

    			score += 10;
    		}
    	}
		return score;
	}

	int Pawn_Evaluation::Evaluate_Pawn_Structure(const Board& board, const PieceColour colour) {
    	int score = 0;
    	const U64 pawnBB = board.Get_Piece_Bitboard(PAWN, colour);

    	for (size_t i = 0; i < FILES.size(); i++) {

    		const int pawnNumbers = Count_Bits(pawnBB & FILES[i]);
    		if (pawnNumbers > 1){ score -= 20 * pawnNumbers; }

    		U64 currentPawn = pawnBB;
    		while (currentPawn) {

    			const int pawnSquare = Get_LS1B_Index(currentPawn);
    			const int pawnFile = pawnSquare % 8;

    			U64 adjacentFile = 0ULL;
    			if (pawnFile > 0){ adjacentFile |= FILES[pawnFile - 1]; }
    			if (pawnFile < 7){ adjacentFile |= FILES[pawnFile + 1]; }

    			// isolated pawn
    			if (!(pawnBB & adjacentFile)) {
    				score -= 15;
    			}

    			// pawn defending
    			if (moveGen.Get_Pawn_Attacks(pawnSquare, colour) & pawnBB) { score += 10; }
    			currentPawn &= currentPawn - 1;

    		}
    	}
    	return score;
    }

	bool Pawn_Evaluation::Is_Passed_Pawn(const Board &board, const int square, const PieceColour colour) {
	    if (!(1ULL << square & board.Get_Piece_Bitboard(PAWN, colour))) { return false; }

    	U64 enemyPawns = board.Get_Piece_Bitboard(PAWN, colour == WHITE ? BLACK : WHITE);
    	const int pawnFile = square % 8;

    	U64 frontSpan = 1ULL << square;
    	if (colour == WHITE) {
    		frontSpan = (frontSpan >> pawnFile) - 1;
    	}
    	else {
    		frontSpan = ~((frontSpan << (8 - pawnFile)) - 1);
    	}
    	enemyPawns &= frontSpan;

    	U64 adjacentFiles = FILES[pawnFile];

    	if (pawnFile > 0){ adjacentFiles |= FILES[pawnFile - 1]; }
    	if (pawnFile < 7){ adjacentFiles |= FILES[pawnFile + 1]; }

    	return (enemyPawns & frontSpan & adjacentFiles) == 0ULL;
	}

	int Pawn_Evaluation::Evaluate_Open_File(const Board &board, const PieceColour colour) {
		int score = 0;
		const U64 friendlyPawns = board.Get_Piece_Bitboard(PAWN, colour);
    	const U64 enemyPawns = board.Get_Piece_Bitboard(PAWN, colour == WHITE ? BLACK : WHITE);

		for (const U64 singleFile : FILES) {
			const U64 friendlyFilePawns = singleFile & friendlyPawns;
			const U64 enemyFilePawns = singleFile & enemyPawns;

			// Half open file
			if (!friendlyFilePawns && enemyFilePawns) {
				score -= HALF_OPEN_FILE_PENALTY;
			}

			// Open file
			else if (!friendlyFilePawns && !enemyFilePawns) {
				score -= OPEN_FILE_PENALTY;
			}
		}
    	return score;
	}

	int Pawn_Evaluation::Evaluate_Passed_Pawn(const Board &board, const PieceColour colour) {
		int score = 0;
		U64 pawns = board.Get_Piece_Bitboard(PAWN, colour);

    	while (pawns) {
    		const int singlePawn = Get_LS1B_Index(pawns);
    		pawns &= pawns - 1;

    		if (Is_Passed_Pawn(board, singlePawn, colour)) {
    			score += PASSED_PAWN_BONUS[singlePawn / 8];
    		}
    	}

    	return score;

	}

	int Pawn_Evaluation::Complete_Pawn_Evaluation(const Board &board, const PieceColour colour) {
		int score = 0;

		score += Evaluate_Passed_Pawn(board, colour);
    	score += Evaluate_Open_File(board, colour);
    	score += Evaluate_Pawn_Shield(board, colour);
    	score += Evaluate_Pawn_Structure(board, colour);

    	return score;
	}

}