#pragma once

#include "l_comms.h"
#include "Orders/Order_enums.h"

#define NMACHINES 8

struct Machine
{
    int count[NPIECES];
    unsigned int total_time;
};

class Factory
{
public:
    Factory() = default;
    Factory(std::array<Machine, NMACHINES> &&other_machines, std::array<int, NPIECES> &&other_unloaded);

    void machined(int cell, int number, piece_t p, unsigned int time);
    void machined(int number, piece_t p, unsigned int time);
    void unloaded(piece_t p);

public:
    std::array<Machine, NMACHINES> machines_stats;
    std::array<int, NPIECES> unload_stats;
};