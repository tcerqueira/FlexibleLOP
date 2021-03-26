#include "MES.h"
#include "udp_server.h"
#include "XmlParser.h"

MES::MES()
{
    scheduler = Scheduler();
    // dispatcher = Dispatcher();
    erp_server = new UdpServer(io_service, LISTEN_PORT);
    store = Storage();
    factory = LOProduction();
}

void MES::start()
{
    MES_INFO("\n###### STARTING ######");
    setUp();
    // TEST
    OrderDoc doc;
    doc.load_file("test/OrderTest.xml");
    // TEST
    MES_INFO("\n###### RUNNING ######");
    run();  
}

void MES::run()
{
    std::thread erpServerThread([this]() {
        MES_TRACE("Starting UDP Server.");
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

        if(node_name == std::string(ORDER_NODE)){
            OrderNode order_node(*it);
            MES_TRACE("Order type: {} ; Number: {} ; Quantity: {}", order_node.name(), order_node.number(), order_node.quantity());
        }
        else if(node_name == std::string(STORAGE_NODE)){

        }
        else if(node_name == std::string(SCHEDULE_NODE)){

        }
        else{
            MES_WARN("Unknown type of order \"{}\".", node_name);
        }
    }
}

void MES::onStorageRequest()
{

}

void MES::onScheduleRequest()
{

}

void MES::onOrder(const OrderNode& order_xml)
{
    
}

MES::~MES()
{
    delete erp_server;
}
