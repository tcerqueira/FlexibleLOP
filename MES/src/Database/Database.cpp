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

void Database::connect()
{
    conn = new pqxx::connection("dbname = sinf2021a13 user = sinf2021a13 password = ljyFftJD host = db.fe.up.pt");
}

Database::Database()
{
    // puts("DB alive");
}

Database::~Database()
{
    delete conn;
}