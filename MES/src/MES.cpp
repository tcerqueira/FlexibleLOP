#include "MES.h"
#include "Orders.h"
#include "udp_server.h"
#include "XmlParser.h"

MES::MES()
{
    scheduler = Scheduler();
    erp_server = new UdpServer(io_service, LISTEN_PORT);
    store = Storage();
    factory = LOProduction();
    // Log::getLogger()->set_level(spdlog::level::info);
}

void MES::start()
{
    MES_INFO("\n###### STARTING ######");
    setUp();
    // TEST
    // OrderDoc doc;
    // doc.load_file("test/OrderTest.xml");
    // TEST
    MES_INFO("\n###### RUNNING ######");
    run();  
}

void MES::run()
{
    std::thread erpServerThread([this]() {
        MES_TRACE("Starting UDP Server. Listening on PORT({}).", LISTEN_PORT);
        io_service.run();
    });

    char buf[50];
    while(1)
    {
        factory.listen();
        factory.send();
        std::cin >> buf;
    }

    erpServerThread.join();
}

int MES::setUp()
{
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
    XmlDoc doc;
    doc.load_buffer_inplace(data, len);

    for (pugi::xml_node_iterator it = doc.root().begin(); it != doc.root().end(); it++)
    {
        auto node_name = std::string(it->name());
        MES_TRACE(node_name);

        // ORDER REQUEST
        if(node_name == std::string(ORDER_NODE)){
            onOrderRequest(*it);
        }
        // STORAGE REQUEST
        else if(node_name == std::string(STORAGE_NODE)){
            onStorageRequest();
        }
        // SCHEDULE REQUEST
        else if(node_name == std::string(SCHEDULE_NODE)){
            onScheduleRequest();
        }
        else{
            MES_WARN("Unknown type of order \"{}\".", node_name);
        }
    }
}

void MES::onOrderRequest(const OrderNode& order_node)
{
    Order* order = Order::Factory(order_node);
    MES_TRACE("Order received: {}.", *order);
    scheduler.addOrder(order);
}

void MES::onStorageRequest()
{

}

void MES::onScheduleRequest()
{

}


MES::~MES()
{
    delete erp_server;
}
