#pragma once
#include "../Board/Board.h"
#include "../Board/Board_Analyser.h"
#include "../Moves/Move.h"
#include "../Moves/Move_Generation.h"
#include "../engine/Search.h"


class Game{

    Board board = Board();
    MoveGeneration Move_Generation;
    PieceColour currentTurn = WHITE;

public:

    explicit Game(const std::string& fen) {
        board.Initialise_From_Fen(fen);
        currentTurn = board.currentTurn;
        Move_Generation = MoveGeneration();
    }

    void Reset_Position(const std::string& fen, const PieceColour colour) {
        board.Initialise_From_Fen(fen);
        currentTurn = colour;
    }

    void Play_Game() {
        std::string input;
        std::string turnString;
        board.Print_Detailed_Board();
        Game_State boardState = board.Get_Game_State();

        while(boardState == Game_State::ACTIVE || boardState == Game_State::CHECK) {
            Board tempBoard = board;
            if (currentTurn == WHITE) {
            Move bestMove = Engine::Search::Find_Best_Move(tempBoard, 4 , WHITE);
                input = Square_To_String(bestMove.Get_From()) + Square_To_String(bestMove.Get_To());
                board.Print_Move_Details(bestMove);
            }

            turnString = currentTurn == WHITE ? "White To Move\n" : "Black To Move\n";
            // std::vector allMoves = Move_Generation.Generate_All_Moves(currentTurn, board);
            // MoveGeneration::Display_All_Moves(allMoves);

            if (currentTurn == BLACK) {
                std::cout << turnString;
                std::cout << "Input your move (or 'undo' to take back last move): ";
                std::cin >> input;
            }

            if(input == "undo") {
                if(board.moveHistory.empty()) {
                    std::cout << "No moves to undo!\n";
                    continue;
                }
                board.Undo_Move(false);
                currentTurn = currentTurn == WHITE ? BLACK : WHITE;
                std::cout << "Move undone\n";
                board.Print_Detailed_Board();
                continue;
            }

            Move userMove = Move_Parsing::Parse_Move(input, currentTurn, board);

            if(Board_Analyser::Make_Move(userMove, false, board)) {

                std::cout << "Move successful\n";
                currentTurn = currentTurn == WHITE ? BLACK : WHITE;
                board.Print_Detailed_Board();
            }
            else {
                board.Print_Detailed_Board();
                std::cout << "Move failed\n";
            }
            boardState = board.Get_Game_State();


        }
        std::cout << "CHECKMATE!" << std::endl;
    }
};
