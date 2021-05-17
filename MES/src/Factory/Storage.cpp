#include "Storage.h"
#include <cstring>

Storage::Storage()
{
    memset(count, 0, sizeof(*count)*NPIECES);
}

Storage::Storage(const int* count)
{
    memcpy(this->count, count, sizeof(*count)*NPIECES);
}


int Storage::countPiece(piece_t type) const
{
    return count[(int)type-1];
}

void Storage::setCount(piece_t type, int count)
{
    this->count[(int)type-1] = count;
}

void Storage::addCount(piece_t type, int count)
{
    this->count[(int)type-1] += count;
}

void Storage::subCount(piece_t type, int count)
{
    this->count[(int)type-1] -= count;
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
