#include "Database.h"

Database* Database::instance{nullptr};
std::mutex Database::mutex;

Database& Database::Get()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new Database();
    }
    return *instance;
}

int Database::connect()
{
    try{
        // TODO: read from config file to not expose secrets
        conn.Connect(_TSA("db.fe.up.pt@sinf2021a13"), _TSA("sinf2021a13"), _TSA("ljyFftJD"), SA_PostgreSQL_Client);
    }
    catch(const SAException& e)
    {
        return 0;
    }

    return 1;
}

Database::Database()
{
    // puts("DB alive");
}

Database::~Database()
{
    // delete conn;
}