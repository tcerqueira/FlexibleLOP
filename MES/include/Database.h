#pragma once

#include "l_comms.h"
#include <mutex>
// #include <pqxx/pqxx>

class Database
{
public:
    Database(Database &other) = delete;
    void operator=(const Database &) = delete;

    static Database& Get();
    int connect();

protected:
    Database();
    ~Database();

private:
    static Database *instance;
    static std::mutex mutex;
    // pqxx::connection *conn;
};