#include "Opc_Client.h"

#include <chrono>
#include <future>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

OpcClient::OpcClient(const std::string &opc_endpoint)
    : endpoint(opc_endpoint)
{
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
}

OpcClient::OpcClient(std::string &&opc_endpoint)
    : endpoint(std::move(opc_endpoint))
{
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
}

int OpcClient::connect()
{
    std::string url = std::string("opc.tcp://") + endpoint;
    connectionStatus = UA_Client_connect(client, url.c_str());
    if (connectionStatus != UA_STATUSCODE_GOOD)
    {
        UA_Client_delete(client);
        return 0;
    }
    return 1;
}

void OpcClient::disconnect()
{
    if (isConnected())
        UA_Client_disconnect(client);
}

bool OpcClient::isConnected()
{
    return (connectionStatus == UA_STATUSCODE_GOOD);
}

#define OPC_LISTEN_ASYNC_MODE 0
int OpcClient::startListening(int t_ms)
{
#if OPC_LISTEN_ASYNC_MODE == 0
    MES_TRACE("Single thread mode.");
#elif OPC_LISTEN_ASYNC_MODE == 1
    MES_TRACE("Boost asio mode.");
#elif OPC_LISTEN_ASYNC_MODE == 2
    MES_TRACE("Fully threaded mode.");
#elif OPC_LISTEN_ASYNC_MODE == 3
    MES_TRACE("Standard async mode.");
#endif
    if (!isConnected())
    {
        return 0;
    }

    isListening = true;
#if OPC_LISTEN_ASYNC_MODE == 3
    std::vector<std::future<void>> futures;

#elif OPC_LISTEN_ASYNC_MODE == 2
    std::vector<std::thread> threads(event_nodes.size());
#endif
    // polling loop
    while (isListening)
    {
        UA_NodeId flag_node;
        auto start = std::chrono::high_resolution_clock::now();

#if OPC_LISTEN_ASYNC_MODE == 1
        boost::asio::thread_pool listen_threadpool(2);
#endif
        // for each subscribed event flag
        for (NodeKey node_key : event_nodes)
        {
            flag_node = UA_NODEID_STRING_ALLOC(node_key.name_space, node_key.id_str.c_str());
            if (checkFlag(flag_node))
            {
#if OPC_LISTEN_ASYNC_MODE == 1
                boost::asio::post(listen_threadpool, [=, this]() {
                    notify({node_key, 0});
                    clearFlag(flag_node);
                });

#elif OPC_LISTEN_ASYNC_MODE == 2
                threads.push_back(std::move(
                    std::thread([=, this]() { // if it doesnt work, comment it out and uncomment code bellow
                        notify({node_key, 0});
                        clearFlag(flag_node);
                })));

#elif OPC_LISTEN_ASYNC_MODE == 3
                futures.push_back(std::move(std::async(std::launch::async, [&, this](){
                    notify({node_key, 0});
                    // clearFlag(flag_node);
                })));

#elif OPC_LISTEN_ASYNC_MODE == 0
                notify({node_key, 0});
                clearFlag(flag_node);
#endif
                // clearFlag(flag_node);
            }
        }
#if OPC_LISTEN_ASYNC_MODE == 3
        for(auto &fut : futures)
            fut.wait();
        futures.clear();

#elif OPC_LISTEN_ASYNC_MODE == 2
        for(std::thread &th : threads)
            if(th.joinable()) th.join();
        threads.clear();

#elif OPC_LISTEN_ASYNC_MODE == 1
        listen_threadpool.join();
#endif
        auto end = std::chrono::high_resolution_clock::now();
        auto sleep_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::milliseconds(t_ms) - (end - start));
        // MES_TRACE(sleep_duration.count());
        std::this_thread::sleep_for(sleep_duration);
    }
    return 1;
}

void OpcClient::notify(opc_evt evt)
{
    for (auto listener : listeners[evt.node])
    {
        listener(evt);
    }
}

void OpcClient::stopListening()
{
    isListening = false;
}

bool OpcClient::checkFlag(UA_NodeId node)
{
    UA_Variant flag;
    UA_Variant_init(&flag);
    if (!readValueBool(node, flag))
    {
        MES_WARN("Could not read flag.");
        return false;
    }
    bool result = *(bool *)flag.data;
    UA_Variant_clear(&flag);
    return result;
}

void OpcClient::clearFlag(UA_NodeId node)
{
    writeValue(node, false);
}

void OpcClient::addListener(NodeKey type, evtHandler handler)
{
    listeners[type].push_back(handler);
    event_nodes.insert(type);
}

// ############################################# READ OPC UA ##############################################
// ########################################################################################################

#define READ_OPCUA(type)\
    UA_StatusCode read_status = UA_Client_readValueAttribute(client, nodeid, &value);\
    if (read_status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[type]))\
    {\
        return 1;\
    };\
    return 0;\

int OpcClient::readValueBool(UA_NodeId nodeid, UA_Variant &value)
{
    std::lock_guard<std::mutex> lck(opc_call_mtx);
    READ_OPCUA(UA_TYPES_BOOLEAN);
}

int OpcClient::readValueInt16(UA_NodeId nodeid, UA_Variant &value)
{
    std::lock_guard<std::mutex> lck(opc_call_mtx);
    READ_OPCUA(UA_TYPES_INT16);
}

int OpcClient::readValueInt32(UA_NodeId nodeid, UA_Variant &value)
{
    std::lock_guard<std::mutex> lck(opc_call_mtx);
    READ_OPCUA(UA_TYPES_INT32);
}

int OpcClient::readValueInt64(UA_NodeId nodeid, UA_Variant &value)
{
    std::lock_guard<std::mutex> lck(opc_call_mtx);
    READ_OPCUA(UA_TYPES_INT64);
}

int OpcClient::readValueUInt16(UA_NodeId nodeid, UA_Variant &value)
{
    std::lock_guard<std::mutex> lck(opc_call_mtx);
    READ_OPCUA(UA_TYPES_UINT16);
}

int OpcClient::readValueUInt32(UA_NodeId nodeid, UA_Variant &value)
{
    std::lock_guard<std::mutex> lck(opc_call_mtx);
    READ_OPCUA(UA_TYPES_UINT32);
}

int OpcClient::readValueUInt64(UA_NodeId nodeid, UA_Variant &value)
{
    std::lock_guard<std::mutex> lck(opc_call_mtx);
    READ_OPCUA(UA_TYPES_UINT64);
}

// ############################################ WRITE OPC UA ##############################################
// ########################################################################################################

int OpcClient::writeValue(UA_NodeId nodeid, UA_Variant &newValue)
{
    std::lock_guard<std::mutex> lck(opc_call_mtx);
    UA_StatusCode write_status = UA_Client_writeValueAttribute(client, nodeid, &newValue);
    if (write_status == UA_STATUSCODE_GOOD)
    {
        return 1;
    }
    return 0;
}

#define WRITE_OPCUA_SCALAR(type)\
    UA_Variant var_value;\
    UA_Variant_init(&var_value);\
    UA_Variant_setScalar(&var_value, &value, &UA_TYPES[type]);\
    int res = writeValue(nodeid, var_value);\
    return res;\

int OpcClient::writeValue(UA_NodeId nodeid, int16_t value)
{
    WRITE_OPCUA_SCALAR(UA_TYPES_INT16);
}

int OpcClient::writeValue(UA_NodeId nodeid, int32_t value)
{
    WRITE_OPCUA_SCALAR(UA_TYPES_INT32);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint16_t value)
{
    WRITE_OPCUA_SCALAR(UA_TYPES_UINT16);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint32_t value)
{
    WRITE_OPCUA_SCALAR(UA_TYPES_UINT32);
}

int OpcClient::writeValue(UA_NodeId nodeid, bool value)
{
    WRITE_OPCUA_SCALAR(UA_TYPES_BOOLEAN);
}

#define WRITE_OPCUA_ARRAY(type)\
    UA_Variant var_value;\
    UA_Variant_init(&var_value);\
    UA_Variant_setArray(&var_value, value, len, &UA_TYPES[UA_TYPES_INT16]);\
    int res = writeValue(nodeid, var_value);\
    return res;\

int OpcClient::writeValue(UA_NodeId nodeid, int16_t *value, int len)
{
    WRITE_OPCUA_ARRAY(UA_TYPES_INT16);
}

int OpcClient::writeValue(UA_NodeId nodeid, int32_t *value, int len)
{
    WRITE_OPCUA_ARRAY(UA_TYPES_INT32);
}

int OpcClient::writeValue(UA_NodeId nodeid, int64_t *value, int len)
{
    WRITE_OPCUA_ARRAY(UA_TYPES_INT64);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint16_t *value, int len)
{
    WRITE_OPCUA_ARRAY(UA_TYPES_UINT16);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint32_t *value, int len)
{
    WRITE_OPCUA_ARRAY(UA_TYPES_UINT32);
}

int OpcClient::writeValue(UA_NodeId nodeid, uint64_t *value, int len)
{
    WRITE_OPCUA_ARRAY(UA_TYPES_UINT64);
}

int OpcClient::writeValue(UA_NodeId nodeid, bool *value, int len)
{
    WRITE_OPCUA_ARRAY(UA_TYPES_BOOLEAN);
}

OpcClient::~OpcClient()
{
    UA_Client_delete(client);
}
