#include "MES.h"
#include <sstream>
#include "Orders/Orders.h"
#include "Database/Database.h"

#define UDP_LISTEN_PORT 54321
#define OPC_LISTEN_PERIOD_MS 100

#define OPC_GLOBAL_NODE(x) { 4, std::string(OPC_GLOBAL_NODE_STR) + x }

MES::MES(const std::string& opc_endpoint)
:   erp_server(io_service, UDP_LISTEN_PORT),
    fct_client(opc_endpoint),
    store((const int[]){100,200,400,800,160,0,0,0,0}),
    scheduler(&store)
{
    std::array<Machine, NMACHINES> machines_array = {{
        {{1,0,0,0,0,0,0,0,0}, 1 },
        {{0,1,0,0,0,0,0,0,0}, 2 },
        {{0,0,1,0,0,0,0,0,0}, 3 },
        {{0,0,0,1,0,0,0,0,0}, 4 },
        {{0,0,0,0,1,0,0,0,0}, 5 },
        {{0,0,0,0,0,1,0,0,0}, 6 },
        {{0,0,0,0,0,0,1,0,0}, 7 },
        {{0,0,0,0,0,0,0,1,0}, 8 }
    }};
    std::array<std::array<int, NPIECES>, NDEST> unloads = {{{1,2,3,4,5,6,7,8,9},{11,12,13,14,15,16,17,18,19},{21,21,23,24,25,26,27,28,29}}};
    factory = Factory(std::move(machines_array), std::move(unloads));
}

MES::MES(std::string&& opc_endpoint)
:   erp_server(io_service, UDP_LISTEN_PORT),
    fct_client(std::move(opc_endpoint)),
    store((const int[]){100,200,400,800,160,0,0,0,0}),
    scheduler(&store)
{
    std::array<Machine, NMACHINES> machines_array = {{
        {{1,0,0,0,0,0,0,0,0}, 1 },
        {{0,1,0,0,0,0,0,0,0}, 2 },
        {{0,0,1,0,0,0,0,0,0}, 3 },
        {{0,0,0,1,0,0,0,0,0}, 4 },
        {{0,0,0,0,1,0,0,0,0}, 5 },
        {{0,0,0,0,0,1,0,0,0}, 6 },
        {{0,0,0,0,0,0,1,0,0}, 7 },
        {{0,0,0,0,0,0,0,1,0}, 8 }
    }};
    std::array<std::array<int, NPIECES>, NDEST> unloads = {{{1,2,3,4,5,6,7,8,9},{11,12,13,14,15,16,17,18,19},{21,21,23,24,25,26,27,28,29}}};
    factory = Factory(std::move(machines_array), std::move(unloads));
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
    erpServerThread.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    MES_INFO("Press to run..."); std::cin.get();

    std::thread opcListenerThread([this]() {
        if(fct_client.startListening(OPC_LISTEN_PERIOD_MS))
            MES_INFO("Listening OPC Server.");
        else {
            MES_FATAL("No connection to OPC Server.");
        }
    });
    opcListenerThread.detach();

    char buf[50];
    while(1)
    {
        std::cin >> buf;
        if(buf[0] == 'x') fct_client.stopListening();
        if(buf[0] == 'f') MES_INFO(factory);
        if(buf[0] == 'c') MES_INFO(store);
        if(buf[0] == 's') MES_INFO(scheduler);
        if(buf[0] == 'q') break;
    }
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
        MES_FATAL("No connection to OPC server.");
    }

    // ####################### UDP HANDLERS ###########################
    // ################################################################
    // set request dispatcher for udp server
    erp_server.setRequestDispatcher([this](char* data, std::size_t len, std::shared_ptr<std::string> response) {
        MES_INFO("{} bytes received from ERP.", len);
        erpRequestDispatcher(data, len, response);
    });
    // set response dispatcher for udp server
    erp_server.setResponseDispatcher([this](std::shared_ptr<std::string> response, std::size_t len) {
        MES_INFO("{} bytes sent to ERP.", len);
    });

    // ####################### OPC HANDLERS ###########################
    // ################################################################
    // set listener to request order C1
    fct_client.addListener(OPC_GLOBAL_NODE("req_orderC1_flag"), [this](opc_evt evt) {
        // MES_INFO("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onSendTransform(1);
    });
    // set listener to request order C2
    fct_client.addListener(OPC_GLOBAL_NODE("req_orderC2_flag"), [this](opc_evt evt) {
        // MES_INFO("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onSendTransform(2);
    });
    // set listener to load order C1
    fct_client.addListener(OPC_GLOBAL_NODE("load_order1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onLoadOrder(P1);
    });
    // set listener to load order C2
    fct_client.addListener(OPC_GLOBAL_NODE("load_order2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onLoadOrder(P2);
    });
    // set listener to start order C1
    fct_client.addListener(OPC_GLOBAL_NODE("start_orderC1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onStartPiece(1);
    });
    // set listener to start order C2
    fct_client.addListener(OPC_GLOBAL_NODE("start_orderC2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onStartPiece(2);
    });
    // set listener to finished order C1
    fct_client.addListener(OPC_GLOBAL_NODE("finish_orderC1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onFinishPiece(1);
    });
    // set listener to finished order C2
    fct_client.addListener(OPC_GLOBAL_NODE("finish_orderC2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onFinishPiece(2);
    });
    // set listener to know factory is ready to receive unload orders
    fct_client.addListener(OPC_GLOBAL_NODE("unload_order_flag"), [this](opc_evt evt) {
        // MES_INFO("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onSendUnload();
    });
    // set listener to order unloaded on PM1
    fct_client.addListener(OPC_GLOBAL_NODE("unload_PM1_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onUnloaded(PM1);
    });
    // set listener to order unloaded on PM2
    fct_client.addListener(OPC_GLOBAL_NODE("unload_PM2_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onUnloaded(PM2);
    });
    // set listener to order unloaded on PM3
    fct_client.addListener(OPC_GLOBAL_NODE("unload_PM3_flag"), [this](opc_evt evt) {
        MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
        onUnloaded(PM3);
    });
    // set listener to machine finished processing
    for(int i=0; i < NMACHINES; i++)
    {
        std::stringstream ss_node; ss_node << "machine_end_flag" << "[" << i << "]";
        fct_client.addListener(OPC_GLOBAL_NODE(ss_node.str()), [i, this](opc_evt evt) {
            MES_TRACE("Notification received on node: n={}:{}", evt.node.name_space, evt.node.id_str);
            onFinishProcessing(i);
        });
    }
    // get storage from db
    for(int i = 0; i< NPIECES; i++)
    {
        store.setCount(static_cast<piece_t> (i+1), Database::Get().getPieceAmount(i+1));
    }
}

MES::~MES()
{
    io_service.stop();
    fct_client.disconnect();
}
