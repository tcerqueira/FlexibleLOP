#pragma once

#include "l_comms.h"
#include <mutex>
#include <SQLAPI.h>
#include "Orders/Orders.h"

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
    std::vector<std::shared_ptr<TransformOrder>> getOrders();
    int insertOrder(std::shared_ptr<TransformOrder> order);
    int deleteOrder(int number);
    std::vector<std::shared_ptr<UnloadOrder>> getUnloads();
    int insertUnload(std::shared_ptr<UnloadOrder> order);
    int deleteUnload(int number);

protected:
    Database();
    ~Database();

private:
    static Database *instance;
    static std::mutex mutex;
    static std::mutex db_call_mutex;
    SAConnection conn;
};

#if DB_ASYNC_MODE == 1
    #define DB_ASYNC(db_thread, func)\
    std::thread db_thread([&]() {\
        func;\
    });

    #define DB_ASYNC_DETACH(db_thread, func)\
    std::thread db_thread([&]() {\
        func;\
    });\
    db_thread.detach();

    #define DB_ASYNC_JOIN(db_thread) db_thread.join()
#else
    #define DB_ASYNC(db_thread, func) func;
    #define DB_ASYNC_DETACH(db_thread, func) func;
    #define DB_ASYNC_JOIN(db_thread)
#endif