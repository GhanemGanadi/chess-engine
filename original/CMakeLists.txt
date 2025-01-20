cmake_minimum_required(VERSION 3.30)
project(chess-engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    add_compile_options(-glldb)
    add_link_options(-glldb)
endif()

add_executable(chess_engine_original main.cpp
        core/Bitboard_Operations.h
        Game/game.h
        Board/Board_Analyser.h
        Board/Board_Analyser.cpp
        Board/Board.h
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
        Board/Zobrist.cpp
        Board/Zobrist.h
)

target_include_directories(chess_engine_original PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(chess_engine_original PRIVATE
        $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
        $<$<CONFIG:Release>:-O3 -march=native -DNDEBUG>
)

if(MSVC)
    target_compile_options(chess_engine_original PRIVATE /W4)
else()
    target_compile_options(chess_engine_original PRIVATE -Wall -Wextra)
endif()