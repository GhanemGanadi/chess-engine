#pragma once
#include "../Board/Board.h"

#define U32 uint32_t
#define U16 uint16_t

struct Book_Entry {
    U64 key;
    U16 move;
    U16 weight;
    U32 learn;
};

class Opening_Book {
    std::vector<Book_Entry> book;

};