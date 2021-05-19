#pragma once

#include "l_comms.h"
#include "Orders/Orders.h"

// #define NPIECES 9

class Storage
{
public:
    Storage();
    Storage(const int* count);

    int countPiece(piece_t type) const;
    void setCount(piece_t type, int count);
    void addCount(piece_t type, int count);
    void subCount(piece_t type, int count);
    int total() const;

private:
    mutable std::mutex mtx;
    int count[NPIECES];
};