cmake_minimum_required(VERSION 3.30)
project(chess-engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    add_compile_options(-glldb)
    add_link_options(-glldb)
endif()

add_executable(chess_engine_optimised main.cpp
        core/board.h
        core/Declarations.h
        core/move.h
        Attack_Tables.cpp
        Attack_Tables.h
        Move_Generator.cpp
        Move_Generator.h
        comparison.h
        engine/Perft.cpp
        engine/Perft.h
)

target_include_directories(chess_engine_optimised PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/core
)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(chess_engine_optimised PRIVATE
            -O0
            -g
            -Wall
            -Wextra
    )
else()
    target_compile_options(chess_engine_optimised PRIVATE
            -O3
            -march=native
            -flto
            -fno-rtti
            -fno-exceptions
            -Wall
            -Wextra
    )

    set_property(TARGET chess_engine_optimised PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(chess_engine_optimised PRIVATE
                /Od
                /Zi
                /DEBUG
        )
    else()
        target_compile_options(chess_engine_optimised PRIVATE
                /O2
                /GL
                /GR
                /EHs-
        )
    endif()
endif()