#include "MES.h"
#include <sstream>
#include "Orders.h"
#include "Utils.h"
#include "Database.h"

MES::MES()
{
    // scheduler = Scheduler();
    erp_server = new UdpServer(io_service, LISTEN_PORT);
    fct_client = opc_client();
    store = Storage((const int[]){1,2,4,8,16,32,64,128,256});
    
    // Log::getLogger()->set_level(spdlog::level::info);
}

void MES::start()
{
    MES_INFO("\n###### STARTING ######");
    setUp();
    MES_INFO("\n###### RUNNING ######");
    run();  
}

void MES::run()
{
    std::thread erpServerThread([this]() {
        MES_INFO("Starting UDP Server. Listening on PORT({}).", LISTEN_PORT);
        io_service.run();
    });

    char buf[50];
    while(1)
    {
        std::cin >> buf;
    }

    erpServerThread.join();
}

void MES::setUp()
{
    // connect to DB
    Database::Get().connect();
    // erp_server->setRequestDispatcher(std::bind(&MES::erpRequestDispatcher, this, std::placeholders::_1);
    erp_server->setRequestDispatcher([this](char* data, std::size_t len, std::shared_ptr<std::string> response) {
        erpRequestDispatcher(data, len, response);
    });

    erp_server->setResponseDispatcher([this](std::shared_ptr<std::string> response, std::size_t len) {
        MES_TRACE("{} bytes sent to ERP.", len);
    });
}

void MES::erpRequestDispatcher(char* data, std::size_t len, std::shared_ptr<std::string> response)
{
    XmlRequest doc;
    doc.load_buffer_inplace(data, len);

    for (pugi::xml_node_iterator it = doc.root().begin(); it != doc.root().end(); it++)
    {
        auto node_name = std::string(it->name());
        // MES_TRACE(node_name);

        // ORDER REQUEST
        if(node_name == std::string(ORDER_NODE)){
            onOrderRequest(*it, response);
        }
        // STORAGE REQUEST
        else if(node_name == std::string(STORAGE_NODE)){
            onStorageRequest(response);
        }
        // SCHEDULE REQUEST
        else if(node_name == std::string(SCHEDULE_NODE)){
            onScheduleRequest(response);
        }
        else{
            MES_WARN("Unknown node name: \"{}\"", node_name);
        }
    }
}

void MES::onOrderRequest(const OrderNode& order_node, std::shared_ptr<std::string> response)
{
    Order* order = MES::OrderFactory(order_node);
    MES_TRACE("Order received: {}.", *order);
    scheduler.addOrder(order);
    // MES_TRACE(scheduler);
}

void MES::onStorageRequest(std::shared_ptr<std::string> response)
{
    std::stringstream ss;
    StorageDoc storeDoc(store);
    storeDoc.save(ss);
    *response = ss.str();
}

void MES::onScheduleRequest(std::shared_ptr<std::string> response)
{
    std::stringstream ss;
    ScheduleDoc scheduleDoc(scheduler);
    scheduleDoc.save(ss);
    *response = ss.str();
}

Order *MES::OrderFactory(const OrderNode &order_node)
{
    time_t receivedAt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    Order *order = nullptr;
    auto node_name = std::string(order_node.name());

    if (node_name == std::string(TRANSF_NODE))
    {
        order = new TransformOrder(order_node.number(), receivedAt, order_node.quantity(), parsePiece(order_node.from()), parsePiece(order_node.to()), order_node.penalty(), order_node.maxdelay());
    }
    else if (node_name == std::string(UNLOAD_NODE))
    {
        order = new UnloadOrder(order_node.number(), receivedAt, order_node.quantity(), parsePiece(order_node.type()), parseDest(order_node.destination()));
    }
    else
    {
        MES_WARN("Unknown type of order: \"{}\"", node_name);
    }

    return order;
}

MES::~MES()
{
    delete erp_server;
}
