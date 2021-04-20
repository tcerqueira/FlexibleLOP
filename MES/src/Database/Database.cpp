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
        conn.Connect(_TSA("demo"), _TSA("guest"), _TSA("secret"), SA_SQLServer_Client);
    }
    catch(const SAException& e)
    {
        MES_ERROR("Error connecting.");
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