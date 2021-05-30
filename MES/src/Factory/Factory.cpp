#include "Factory.h"

Factory::Factory(std::array<Machine, NMACHINES> &&other_machines, std::array<std::array<int, NPIECES>, NDEST> &&other_unloaded)
: machines_stats(std::move(other_machines)), unload_stats(std::move(other_unloaded))
{
}

void Factory::machined(int cell, int number, piece_t p, unsigned int time)
{
    machined((cell-1)*4+number, p, time);
}

void Factory::machined(int number, piece_t p, unsigned int time)
{
    machines_stats[number].count[(int)p-1]++;
    machines_stats[number].total_time += time;
}

void Factory::unloaded(piece_t p, dest_t d)
{
    unload_stats[(int)d-1][p-1]++;
}
