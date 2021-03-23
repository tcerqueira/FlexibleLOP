#pragma once

#include "l_comms.h"
#include "Orders.h"

#define NPIECES 9

class Storage
{
public:
    int countPiece(piece_t type);
    int total();

private:
    int count[NPIECES];
};