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

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Storage &stg);

    // operator overload
    int operator[](piece_t piece);

private:
    mutable std::mutex mtx;
    int count[NPIECES];
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Storage &stg)
{
    os << std::endl << "==== Storage ====" << std::endl;
    for(int i = 0; i < NPIECES; i++)
    {
        os << "P" << i+1 << ": " << stg.count[i]<< "\n";
    }
    return os;
}