#pragma once

#include "l_comms.h"
#include <mutex>
#include <SQLAPI.h>

class Database
{
public:
    Database(Database &other) = delete;
    void operator=(const Database &) = delete;

    static Database& Get();
    int connect();
    int updateStorage(int piece_type, int amount);
    int getPieceAmount(int piece_type);
    int* getStorage();
    int updateMachine(int id_mac, int total_time);
    int getMachine(int id_mac);
    int updateMachineStat(int id_mac, int piece_type, int piece_count);
    int getMachinePieceCount(int id_mac, int piece_type);

protected:
    Database();
    ~Database();

private:
    static Database *instance;
    static std::mutex mutex;
    SAConnection conn;
};