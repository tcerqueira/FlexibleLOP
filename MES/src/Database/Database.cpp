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
    // try{
    //     conn = new pqxx::connection("dbname = sinf2021a13 user = sinf2021a13 password = ljyFftJD host = db.fe.up.pt");
    //     return 1;
    // }
    // catch(const std::exception &e){
    //     MES_ERROR("Connection Error: {}", e.what());
    //     return 0;
    // }
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