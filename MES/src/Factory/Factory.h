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
    Factory(std::array<Machine, NMACHINES> &&other_machines, std::array<std::array<int, NPIECES>, NDEST> &&other_unloaded);

    void machined(int cell, int number, piece_t p, unsigned int time);
    void machined(int number, piece_t p, unsigned int time);
    void unloaded(piece_t p, dest_t d);

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Factory &fct);

public:
    std::array<Machine, NMACHINES> machines_stats;
    std::array<std::array<int, NPIECES>, NDEST> unload_stats;
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Factory &fct)
{
    os << std::endl << "==== Factory ====" << std::endl;
    int i = 0;
    for(auto mac : fct.machines_stats)
    {
        os << "Machine " << i << ": [ ";
        for(int j=0; j < NPIECES; j++)
        {
            os << "P" << j+1 << ":" << mac.count[j] << " ";
        }
        os << "] - Total time: " << mac.total_time << std::endl;
        i++;
    }
    // unload stats print
    return os;
}