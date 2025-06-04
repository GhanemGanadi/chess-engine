cmake_minimum_required(VERSION 3.13)
project(chess-engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    add_compile_options(-glldb)
    add_link_options(-glldb)
endif()

add_executable(chess_engine src/main.cpp
        src/core/board.h
        src/core/Declarations.h
        src/core/move.h
        src/Attack_Tables.cpp
        src/Attack_Tables.h
        src/Move_Generator.cpp
        src/Move_Generator.h
        src/comparison.h
        src/engine/Perft.cpp
        src/engine/Perft.h
        src/engine/Game.cpp
        src/engine/Game.h
)

target_include_directories(chess_engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        src/core
)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(chess_engine PRIVATE
            -O0
            -g
            -Wall
            -Wextra
    )
else()
    target_compile_options(chess_engine PRIVATE
            -O3
            -march=native
            -flto
            -fno-rtti
            -Wall
            -Wextra
    )

    set_property(TARGET chess_engine PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(chess_engine PRIVATE
                /Od
                /Zi
                /DEBUG
        )
    else()
        target_compile_options(chess_engine PRIVATE
                /O2
                /GL
                /GR
                /EHs-
        )
    endif()
endif()