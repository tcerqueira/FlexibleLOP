#include "Storage.h"

Storage::Storage()
{
    for(int i=0; i < NPIECES; i++)
        count[i] = 0;
}

Storage::Storage(const int* count)
{
    for(int i=0; i < NPIECES; i++)
        this->count[i] = count[i];
}


int Storage::countPiece(piece_t type) const
{
    return count[type-1];
}

void Storage::setCount(piece_t type, int count)
{
    this->count[type-1] = count;
}

int Storage::total() const
{
    int total = 0;
    for(int i=0; i < NPIECES; i++)
    {
        total += count[i];
    }
    return total;
}
