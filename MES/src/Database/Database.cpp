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
        SACommand search_path(&conn, _TSA("SET search_path TO mes"));
        search_path.Execute();
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

int Database::updateStorage(int piece_type, int amount)
{
    SACommand update(&conn, _TSA("UPDATE PieceStorage SET amount = :1 WHERE piece_type = :2"));
    update << (long) amount << (long) piece_type;
    try{
        update.Execute();
    }
    catch(const SAException& e)
    {
        MES_TRACE("{}", e.ErrText().GetMultiByteChars());
        MES_TRACE("ERROR updating db storage piece: {} amount: {}", piece_type, amount);
        return 0;
    }

    return 1;
}

int Database::getPieceAmount(int piece_type)
{
    if(piece_type > 9 || piece_type < 1) return -1;

    SACommand get(&conn, _TSA("SELECT amount FROM PieceStorage WHERE piece_type = :1"));
    get << (long) piece_type;
    get.Execute();
    int amount;
    int i = 0;
    while(get.FetchNext())
    {
        long amountL = get[1];
        amount = (int) amountL;
        i++;
    }
    return amount;
}

int* Database::getStorage()
{
    SACommand get(&conn, _TSA("SELECT * FROM PieceStorage"));
    get.Execute();
    int *count = (int*) malloc(sizeof(int)*9);
    while(get.FetchNext())
    {
        long piece_typeL = get[1];
        int piece_type = (int) piece_typeL;
        long amountL = get[2];
        int amount = (int) amountL;

        if(piece_type > 9 || piece_type < 1) continue;

        count[piece_type-1] = amount;
    }
    return count;
}

int Database::updateMachine(int id_mac, int total_time)
{
    SACommand update(&conn, _TSA("UPDATE Machine SET total_time = :1 WHERE id_mach = :2"));
    update << (long) total_time << (long) id_mac;
    try{
        update.Execute();
    }
    catch(const SAException& e)
    {
        return 0;
    }

    return 1;
}

int Database::getMachine(int id_mac)
{
    if(id_mac > 7 || id_mac < 0) return -1;

    SACommand get(&conn, _TSA("SELECT total_time FROM Machine WHERE id_mac = :1"));
    get << (long) id_mac;
    get.Execute();
    int total_time;
    int i = 0;
    while(get.FetchNext())
    {
        if(i > 0) return -1;
        long total_timeL = get[1];
        total_time = (int) total_timeL;
        i++;
    }
    return total_time;
}

int Database::updateMachineStat(int id_mac, int piece_type, int piece_count)
{
    SACommand update(&conn, _TSA("UPDATE MachineStat SET piece_count = :1 WHERE id_mach = :2 AND piece_type = :3"));
    update << (long) piece_count << (long) id_mac << (long) piece_type;
    try{
        update.Execute();
    }
    catch(const SAException& e)
    {
        return 0;
    }

    return 1;
}

int Database::getMachinePieceCount(int id_mac, int piece_type)
{
    if(id_mac > 7 || id_mac < 0) return -1;

    SACommand get(&conn, _TSA("SELECT piece_count FROM MachineStat WHERE id_mac = :1 AND piece_type = :2"));
    get << (long) id_mac << (long) piece_type;
    get.Execute();
    int piece_count;
    int i = 0;
    while(get.FetchNext())
    {
        if(i > 0) return -1;
        long piece_countL = get[1];
        piece_count = (int) piece_countL;
        i++;
    }
    return piece_count;
}