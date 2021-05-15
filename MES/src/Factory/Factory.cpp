#include "Factory.h"

Factory::Factory(std::array<Machine, NMACHINES> &&other_machines, std::array<int, NPIECES> &&other_unloaded)
: machines_stats(std::move(other_machines)), unload_stats(std::move(other_unloaded))
{
}

void Factory::machined(int cell, int number, piece_t p, unsigned int time)
{
    machined((cell-1)*2+number, p, time);
}

void Factory::machined(int number, piece_t p, unsigned int time)
{
    machines_stats[number].count[p-1]++;
    machines_stats[number].total_time += time;
}

void Factory::unloaded(piece_t p)
{
    unload_stats[p-1]++;
}
