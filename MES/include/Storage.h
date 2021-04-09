#pragma once

#include "l_comms.h"
#include "Orders.h"

#define NPIECES 9

class Storage
{
public:
    Storage();
    Storage(const int* count);

    int countPiece(piece_t type) const;
    void setCount(piece_t type, int count);
    int total() const;

private:
    int count[NPIECES];
};