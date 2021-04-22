#include "MES.h"
#include <sstream>
#include "Orders.h"
#include "Utils.h"
#include "Database.h"

MES::MES()
{
    // scheduler = Scheduler();
    erp_server = new UdpServer(io_service, UDP_LISTEN_PORT);
    fct_client = new OpcClient();
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
    fct_client->connect("LAPTOP-6MKI0KJB:4840");

    std::thread erpServerThread([this]() {
        MES_INFO("Starting UDP Server. Listening on PORT({}).", UDP_LISTEN_PORT);
        io_service.run();
    });

    std::thread opcListenerThread([this]() {
        MES_INFO("Listening OPC Server.");
        fct_client->startListening(OPC_LISTEN_PERIOD_MS);
    });

    char buf[50];
    int aux=1;
    while(1)
    {
        std::cin >> buf;
        // MES_TRACE(scheduler);
        if(buf[0] == 'x') fct_client->stopListening();

        if(buf[0] == 'a')
        {
            aux++;
            UA_Variant *test = UA_Variant_new();
            UA_Variant_setScalarCopy(test, &aux, &UA_TYPES[UA_TYPES_INT16]);
            fct_client->writevalue(UA_NODEID_STRING(4, "|var|CODESYS Control Win V3 x64.Application.GVL.test") , test);
            UA_Variant_delete(test);
        }
    }

    erpServerThread.join();
    opcListenerThread.join();
}

void MES::setUp()
{
    // connect to DB
    if(!Database::Get().connect()){
        // TODO: connection fails
        MES_ERROR("Could not connect to Database.");
    }
    // set request dispatcher for udp server
    erp_server->setRequestDispatcher([this](char* data, std::size_t len, std::shared_ptr<std::string> response) {
        erpRequestDispatcher(data, len, response);
    });
    // set response dispatcher for udp server
    erp_server->setResponseDispatcher([this](std::shared_ptr<std::string> response, std::size_t len) {
        MES_TRACE("{} bytes sent to ERP.", len);
    });
    // set listener to request order
    fct_client->addListener(REQ_ORDER, [this](opc_evt evt) {
        MES_TRACE("Listened Request Order. (data={})", evt.data);
    });
    // set listener to order beginning
    fct_client->addListener(ORDER_BEGIN, [this](opc_evt evt) {
        MES_TRACE("Listened Order Begin. (data={})", evt.data);
    });
    // set listener to order ending
    fct_client->addListener(ORDER_END, [this](opc_evt evt) {
        MES_TRACE("Listened Order End. (data={})", evt.data);
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

    auto t_order = dynamic_cast<TransformOrder*>(order);
    auto u_order = dynamic_cast<UnloadOrder*>(order);
    if(t_order != nullptr)
    {
        scheduler.addTransform(t_order);
    }
    else if(u_order != nullptr)
    {
        scheduler.addUnload(u_order);
    }
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
    delete fct_client;
}
