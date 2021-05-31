#include "Database.h"
#include "Orders/Orders.h"

Database* Database::instance{nullptr};
std::mutex Database::mutex;
std::mutex Database::db_call_mutex;

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
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return 0;
    SACommand update(&conn, _TSA("UPDATE PieceStorage SET amount = :1 WHERE piece_type = :2"));
    update << (long) amount << (long) piece_type;
    try{
        update.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrText().GetMultiByteChars());
        return 0;
    }

    return 1;
}

int Database::getPieceAmount(int piece_type)
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return -1;

    if(piece_type > 9 || piece_type < 1) return -1;

    SACommand get(&conn, _TSA("SELECT amount FROM PieceStorage WHERE piece_type = :1"));
    get << (long) piece_type;
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }
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

// To Test
int* Database::getStorage()
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return nullptr;

    SACommand get(&conn, _TSA("SELECT * FROM PieceStorage"));
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }
    int *count = new int[9]; //(int*) malloc(sizeof(int)*9);
    while(get.FetchNext())
    {
        int piece_type = get[1].asInt64();
        int amount = get[2].asInt64();

        if(piece_type > 9 || piece_type < 1) continue;

        count[piece_type-1] = amount;
    }
    return count;
}

int Database::updateMachine(int id_mac, int total_time)
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return 0;

    SACommand update(&conn, _TSA("UPDATE Machine SET total_time = :1 WHERE id_mac = :2"));
    update << (long) total_time << (long) id_mac;
    try{
        update.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }

    return 1;
}

int Database::getMachine(int id_mac)
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return -1;

    if(id_mac > 7 || id_mac < 0) return -1;

    SACommand get(&conn, _TSA("SELECT total_time FROM Machine WHERE id_mac = :1"));
    get << (long) id_mac;
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }
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
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return 0;

    SACommand update(&conn, _TSA("UPDATE MachineStat SET piece_count = :1 WHERE id_mac = :2 AND piece_type = :3"));
    update << (long) piece_count << (long) id_mac << (long) piece_type;
    try{
        update.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }

    return 1;
}

int Database::getMachinePieceCount(int id_mac, int piece_type)
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return -1;

    if(id_mac > 7 || id_mac < 0) return -1;

    SACommand get(&conn, _TSA("SELECT piece_count FROM MachineStat WHERE id_mac = :1 AND piece_type = :2"));
    get << (long) id_mac << (long) piece_type;
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }
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

std::vector<std::shared_ptr<TransformOrder>> Database::getOrders()
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    std::vector<std::shared_ptr<TransformOrder>> order;
    if(!conn.isConnected()) return order;
    SACommand get(&conn, _TSA("SELECT * FROM TransformOrder"));
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return order;
    }
    
    while(get.FetchNext())
    {
        std::shared_ptr<TransformOrder> aux = std::make_shared<TransformOrder>(get.Field(_TSA("id_number")).asInt64(), get.Field(_TSA("received_at")), get.Field(_TSA("total_amount")).asInt64(), (piece_t) get.Field(_TSA("piece_initial")).asInt64(), (piece_t) get.Field(_TSA("piece_final")).asInt64(), get.Field(_TSA("penalty_per_day")).asInt64(), get.Field(_TSA("max_delay")).asInt64());
        aux->setDone(get.Field(_TSA("done_amount")).asInt64());
        order.push_back(aux);
    }
    return order;
}

int Database::insertOrder(std::shared_ptr<TransformOrder> order)
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return 0;

    SACommand get(&conn, _TSA("SELECT id_number FROM TransformOrder WHERE id_number = :1"));
    get << (long) order->getId();
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        // return 0;
    }
    if(get.FetchNext())
    {
        MES_WARN("DB insertOrder: Order id ({}) already exists", order->getId());
        return 0;
    }
    SACommand insert(&conn, _TSA("INSERT INTO TransformOrder (id_number, piece_initial, piece_final, total_amount, done_amount, doing_amount, sent_at, received_at, max_delay, penalty_per_day, started_at, finished_at, penalty) VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13)"));
    insert << (long) order->getId() << (long) order->getInitial() << (long) order->getFinal() << (long) order->getQuantity() << (long) order->getDone() << (long) order->getDoing() << (long) order->getTimeSent() << (long) order->getTimeRcv() << (long) order->getMaxDelay() << (long) order->getDailyPenalty() << (long) order->getTimeStarted() << (long) order->getTimeFinished() << (long) order->getPenalty();
    try
    {
        insert.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }
    return 1;
}

int Database::deleteOrder(int number)
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return 0;

    SACommand get(&conn, _TSA("SELECT id_number FROM TransformOrder WHERE id_number = :1"));
    get << (long) number;
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        //MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        // return 0;
    }
    if(!get.FetchNext())
    {
        MES_WARN("[DB deleteOrder: Order id ({}) doesn't exist in db aborting", number);
        return 0;
    }

    SACommand del(&conn, _TSA("DELETE FROM TransformOrder WHERE id_number = :1"));
    del << (long) number;
    try
    {
        del.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }

    return 1;
}

std::vector<std::shared_ptr<UnloadOrder>> Database::getUnloads()
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    std::vector<std::shared_ptr<UnloadOrder>> order;
    if(!conn.isConnected()) return order;
    SACommand get(&conn, _TSA("SELECT * FROM UnloadOrder"));
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        //MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return order;
    }

    
    while(get.FetchNext())
    {
        std::shared_ptr<UnloadOrder> aux = std::make_shared<UnloadOrder>(get.Field(_TSA("id_number")).asInt64(), get.Field(_TSA("received_at")), get.Field(_TSA("quantity")).asInt64(), (piece_t) get.Field(_TSA("piece_type")).asInt64(), (dest_t) get.Field(_TSA("destination")).asInt64());
        order.push_back(aux);
    }
    return order;
}

int Database::insertUnload(std::shared_ptr<UnloadOrder> order)
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return 0;

    SACommand get(&conn, _TSA("SELECT id_number FROM UnloadOrder WHERE id_number = :1"));
    get << (long) order->getId();
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        //MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        // return 0;
    }
    if(get.FetchNext())
    {
        MES_WARN("DB insertOrder: Order id ({}) already exists aborting", order->getId());
        return 0;
    }

    SACommand insert(&conn, _TSA("INSERT INTO UnloadOrder (id_number, piece_type, destination, quantity, received_at) VALUES (:1, :2, :3, :4, :5)"));
    insert << (long) order->getId() << (long) order->getPiece() << (long) order->getDest() << (long) order->getQuantity() << (long) order->getTimeRcv();

    try
    {
        insert.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }

    return 1;
}

int Database::deleteUnload(int number)
{
    std::lock_guard<std::mutex> lock(db_call_mutex);
    if(!conn.isConnected()) return 0;

    SACommand get(&conn, _TSA("SELECT id_number FROM UnloadOrder WHERE id_number = :1"));
    get << (long) number;
    try
    {
        get.Execute();
    }
    catch(const SAException& e)
    {
        //MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        // return 0;
    }
    if(!get.FetchNext())
    {
        MES_WARN("DB deleteOrder: Unload id ({}) doesn't exist in db", number);
        return 0;
    }

    SACommand del(&conn, _TSA("DELETE FROM UnloadOrder WHERE id_number = :1"));
    del << (long) number;
    try
    {
        del.Execute();
    }
    catch(const SAException& e)
    {
        MES_ERROR("{}", e.ErrMessage().GetMultiByteChars());
        return 0;
    }

    return 1;
}