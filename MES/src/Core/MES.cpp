#include "MES.h"
#include <sstream>
#include "Orders/Orders.h"
#include "Database/Database.h"

#define UDP_LISTEN_PORT 54321
#define OPC_LISTEN_PERIOD_MS 10000

#define OPC_GLOBAL_NODE(x) { 4, std::string(OPC_GLOBAL_NODE_STR) + x }

MES::MES(const std::string& opc_endpoint)
:   erp_server(io_service, UDP_LISTEN_PORT),
    fct_client(opc_endpoint),
    store((const int[]){1,2,4,8,16,32,64,128,256})
{
}

MES::MES(std::string&& opc_endpoint)
:   erp_server(io_service, UDP_LISTEN_PORT),
    fct_client(std::move(opc_endpoint)),
    store((const int[]){1,2,4,8,16,32,64,128,256})
{
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
        MES_INFO("Starting UDP Server. Listening on PORT({}).", UDP_LISTEN_PORT);
        io_service.run();
    });

    std::thread opcListenerThread([this]() {
        MES_INFO("Listening OPC Server.");
        fct_client.startListening(OPC_LISTEN_PERIOD_MS);
    });

    char buf[50];
    while(1)
    {
        std::cin >> buf;
        // MES_TRACE(scheduler);
        if(buf[0] == 'x') fct_client.stopListening();
    }

    erpServerThread.join();
    opcListenerThread.join();
}

void MES::setUp()
{
    // ####################### CONNECTIONS ############################
    // ################################################################
    // connect to DB
    if(!Database::Get().connect()){
        // TODO: connection fails
        MES_FATAL("No connection to Database.");
    }
    // connect to OPC server
    if(!fct_client.connect()){
        MES_FATAL("No connection to OPC server. Aborting.");
    }

    // ####################### UDP HANDLERS ###########################
    // ################################################################
    // set request dispatcher for udp server
    erp_server.setRequestDispatcher([this](char* data, std::size_t len, std::shared_ptr<std::string> response) {
        erpRequestDispatcher(data, len, response);
    });
    // set response dispatcher for udp server
    erp_server.setResponseDispatcher([this](std::shared_ptr<std::string> response, std::size_t len) {
        MES_TRACE("{} bytes sent to ERP.", len);
    });

    // ####################### OPC HANDLERS ###########################
    // ################################################################
    // set listener to request order C1
    fct_client.addListener(OPC_GLOBAL_NODE("req_orderC1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onSendTransform(1);
    });
    // set listener to request order C2
    fct_client.addListener(OPC_GLOBAL_NODE("req_orderC2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onSendTransform(2);
    });
    // set listener to load order C1
    fct_client.addListener(OPC_GLOBAL_NODE("load_order1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onLoadOrder();
    });
    // set listener to load order C2
    fct_client.addListener(OPC_GLOBAL_NODE("load_order2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onLoadOrder();
    });
    // set listener to start order C1
    fct_client.addListener(OPC_GLOBAL_NODE("start_orderC1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onStartOrder();
    });
    // set listener to start order C2
    fct_client.addListener(OPC_GLOBAL_NODE("start_orderC2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onStartOrder();
    });
    // set listener to finished order C1
    fct_client.addListener(OPC_GLOBAL_NODE("finish_orderC1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onFinishOrder();
    });
    // set listener to finished order C2
    fct_client.addListener(OPC_GLOBAL_NODE("finish_orderC2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onFinishOrder();
    });
    // set listener to order unloaded on PM1
    fct_client.addListener(OPC_GLOBAL_NODE("unload_PM1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onUnloaded();
    });
    // set listener to order unloaded on PM2
    fct_client.addListener(OPC_GLOBAL_NODE("unload_PM2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onUnloaded();
    });
    // set listener to order unloaded on PM3
    fct_client.addListener(OPC_GLOBAL_NODE("unload_PM3_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onUnloaded();
    });
    // set listener to machine finished processing
    fct_client.addListener(OPC_GLOBAL_NODE("machine_end_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onFinishProcessing();
    });
}
