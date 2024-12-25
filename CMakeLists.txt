cmake_minimum_required(VERSION 3.29)
project(chess-engine)

set(CMAKE_CXX_STANDARD 23)

if(APPLE)
    add_compile_options(-glldb)
    add_link_options(-glldb)
endif()

add_executable(bitboard main.cpp
        core/Bitboard_Operations.h
        Game/game.h
        Board/Board_Analyser.h
        Board/Board.h
        Board/Board_Analyser.cpp
        Moves/Move_Generation.h
        Moves/Move.h
        Moves/Tables.h
        Moves/Tables.cpp
        Moves/Move.cpp
        engine/Evaluation.h
        engine/Search.h
        engine/Evaluation.cpp
        engine/Search.cpp

        engine/perft.cpp
        engine/perft.h

        engine/Phases.cpp
        engine/Phases.h

        engine/Pawns.cpp
        engine/Pawns.h
        engine/King.h
        engine/King.cpp
        Moves/Move_Generation.cpp
        core/Book.cpp
        core/Book.h
        core/PGN_Parser.cpp
        core/PGN_Parser.h
        config.h

)