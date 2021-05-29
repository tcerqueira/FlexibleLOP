#include "MES.h"
#include "Database/Database.h"

static std::mutex cell_mtx;

int writeTransform(OpcClient &opc_client, std::shared_ptr<SubOrder> order, int cell)
{
    std::stringstream ss_node;
    ss_node << "orders_C" << cell;

    std::string node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].init_p"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->init_p))   return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].final_p"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->final_p))  return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].orderID"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->orderID))  return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].quantity"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->quantity)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].to_do"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->to_do))    return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].done"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->done)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].tool_set"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->tool_set, TOOLSET_BUFLEN)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].piece_seq"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->piece_seq, PIECESEQ_BUFLEN)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].path"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->path, PATH_BUFLEN))  return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].tool_time"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->tool_time, TOOLTIME_BUFLEN)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].warehouse_intermidiate"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->warehouse_intermediate))   return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + std::string(ss_node.str()) + std::string("[1].piece_intermidiate"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order->piece_intermediate))   return 0;

    return 1;
}

void MES::onSendTransform(int cell)
{
    std::lock_guard<std::mutex> lock(cell_mtx);
    auto next_order = scheduler.requestOrderCell(cell);
    if(next_order == nullptr)
        return;

    // Write to factory
    if(!writeTransform(fct_client, next_order, cell)){
        MES_ERROR("Could not send Transform Order.");
        return;
    }
    store.subCount((piece_t)(int)next_order->init_p, next_order->quantity);
    MES_INFO("Transform Order sent on cell {}: {}", cell, *next_order);
}

struct opc_unload
{
    uint16_t type_t;
    int16_t dest;
    int16_t quant;
};

int writeUnload(OpcClient &opc_client, const opc_unload &order)
{
    std::string unload_node = "unload_orders";

    std::string node = std::move(std::string(OPC_GLOBAL_NODE_STR) + unload_node + std::string("[1].type_t"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.type_t)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + unload_node + std::string("[1].dest"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.dest)) return 0;

    node = std::move(std::string(OPC_GLOBAL_NODE_STR) + unload_node + std::string("[1].quant"));
    if(!opc_client.writeValue(UA_NODEID_STRING_ALLOC(4, node.c_str()), order.quant)) return 0;

    return 1;
}

void MES::onSendUnload()
{
    std::lock_guard<std::mutex> lock(cell_mtx);
    std::shared_ptr<UnloadOrder> next_unload = scheduler.requestUnload();
    if(next_unload == nullptr)
        return;

    opc_unload opc_u = {(uint16_t)next_unload->getPiece(), (int16_t)next_unload->getDest(), (int16_t)next_unload->getQuantity()};

    if(!writeUnload(fct_client, opc_u)){
        MES_ERROR("Could not send Unload Order.");
        return;
    }
    store.subCount(next_unload->getPiece(), next_unload->getQuantity());
    // TODO async query
    // Database::Get().updateStorage((int) next_unload->getPiece(), store.countPiece(next_unload->getPiece()));
    MES_INFO("Unload sent: {}", *next_unload);
}

void MES::onLoadOrder(piece_t piece)
{
    // Update Storage
    MES_INFO("Piece {} loaded.", (int)piece);
    store.addCount(piece, 1);
}

void MES::onStartPiece(int cell)
{
    // Update scheduler by id
    std::stringstream ss_node;
    ss_node << OPC_GLOBAL_NODE_STR << "starting_piece_numberC" << cell;
    const std::string &str_node = ss_node.str();

    UA_Variant number_var;
    UA_Variant_init(&number_var);
    if(!fct_client.readValueInt16(UA_NODEID_STRING_ALLOC(4, str_node.c_str()), number_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_node);
        return;
    }
    int number = *(int*)number_var.data;
    MES_TRACE("Piece of order {} started on cell {}.", number, cell);
    scheduler.updatePieceStarted(cell, number);
    UA_Variant_clear(&number_var);
}

void MES::onFinishPiece(int cell)
{
    // Update scheduler by id
    std::stringstream ss_node_number, ss_node_type;
    ss_node_number << OPC_GLOBAL_NODE_STR << "finishing_piece_numberC" << cell;
    ss_node_type << OPC_GLOBAL_NODE_STR << "finishing_piece_typeC" << cell;
    const std::string &str_node_number = ss_node_number.str();
    const std::string &str_node_type = ss_node_type.str();

    UA_Variant number_var, type_var;
    UA_Variant_init(&number_var);
    UA_Variant_init(&type_var);
    if(!fct_client.readValueInt16(UA_NODEID_STRING_ALLOC(4, str_node_number.c_str()), number_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_node_number);
        return;
    }
    if(!fct_client.readValueUInt16(UA_NODEID_STRING_ALLOC(4, str_node_type.c_str()), type_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_node_type);
        return;
    }
    int number = *(int*)number_var.data;
    piece_t type = (piece_t)(int)*(uint16_t*)type_var.data;
    MES_TRACE("Piece type {} of order {} finished on cell {}.", (int)type, number, cell);
    store.addCount(type, 1);
    scheduler.updatePieceFinished(cell, number);
    UA_Variant_clear(&number_var);
}

void MES::onUnloaded(dest_t dest)
{
    // Update stats
    std::stringstream ss_node;
    ss_node << OPC_GLOBAL_NODE_STR << "unloaded_PM" << (int)dest << "_type";
    const std::string &str_node = ss_node.str();

    UA_Variant type_var;
    UA_Variant_init(&type_var);
    if(!fct_client.readValueUInt16(UA_NODEID_STRING_ALLOC(4, str_node.c_str()), type_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_node);
        return;
    }
    piece_t unload_piece = (piece_t)(int)*(uint16_t*)type_var.data;
    MES_INFO("Piece unloaded of type {} on destination {}.", (int)unload_piece, (int)dest);
    factory.unloaded(unload_piece, dest);
    UA_Variant_clear(&type_var);
}

void MES::onFinishProcessing(int machine)
{
    // Update stats
    std::stringstream ss_type_node; ss_type_node << OPC_GLOBAL_NODE_STR << "machined_type" << "[" << machine << "]";
    std::stringstream ss_time_node; ss_time_node << OPC_GLOBAL_NODE_STR << "machined_time" << "[" << machine << "]";

    const std::string &str_type_node = ss_type_node.str();
    const std::string &str_time_node = ss_time_node.str();

    UA_Variant type_var, time_var;

    UA_Variant_init(&type_var);
    if(!fct_client.readValueInt16(UA_NODEID_STRING_ALLOC(4, str_type_node.c_str()), type_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_type_node);
        return;
    }

    UA_Variant_init(&time_var);
    if(!fct_client.readValueUInt64(UA_NODEID_STRING_ALLOC(4, str_time_node.c_str()), time_var)) {
        MES_ERROR("Could not read from node \"{}\".", str_time_node);
        return;
    }

    piece_t machined_piece = (piece_t)(int)*(int16_t*)type_var.data;
    unsigned int machined_time = (unsigned int)*(uint64_t*)time_var.data;
    MES_TRACE("Machine {}: type {} and time {}.", machine, (int)machined_piece, machined_time);
    if(machined_piece < 1 || machined_piece > NPIECES) return;
    factory.machined(machine, machined_piece, machined_time);
    UA_Variant_clear(&type_var);
    UA_Variant_clear(&time_var);
}

// ############################################ AUXILIAR FUNCTIONS #################################################
// #################################################################################################################
