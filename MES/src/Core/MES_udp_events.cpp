#include "MES.h"
#include "Utils.h"

void MES::erpRequestDispatcher(char* data, std::size_t len, std::shared_ptr<std::string> response)
{
    XmlRequest doc;
    doc.load_buffer_inplace(data, len);

    for (pugi::xml_node_iterator it = doc.root().begin(); it != doc.root().end(); it++)
    {
        auto node_name = std::string(it->name());

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
            MES_WARN("Unknown request node name: \"{}\"", node_name);
        }
    }
    scheduler.schedule();
}

std::shared_ptr<Order> MES::OrderFactory(const OrderNode &order_node)
{
    time_t receivedAt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::shared_ptr<Order> order;
    auto node_name = std::string(order_node.name());

    if (node_name == std::string(TRANSF_NODE))
    {
        order = std::make_shared<TransformOrder>(order_node.number(), receivedAt, order_node.quantity(), parsePiece(order_node.from()), parsePiece(order_node.to()), order_node.penalty(), order_node.maxdelay());
    }
    else if (node_name == std::string(UNLOAD_NODE))
    {
        order = std::make_shared<UnloadOrder>(order_node.number(), receivedAt, order_node.quantity(), parsePiece(order_node.type()), parseDest(order_node.destination()));
    }
    else
    {
        MES_WARN("Unknown type of order: \"{}\"", node_name);
    }

    return order;
}

void MES::onOrderRequest(const OrderNode& order_node, std::shared_ptr<std::string> response)
{
    std::shared_ptr<Order> order = MES::OrderFactory(order_node);
    MES_TRACE("Order received: {}.", *order);

    auto t_order = std::dynamic_pointer_cast<TransformOrder>(order);
    auto u_order = std::dynamic_pointer_cast<UnloadOrder>(order);
    if(t_order != nullptr)
    {
        scheduler.addTransform(t_order);
    }
    else if(u_order != nullptr)
    {
        scheduler.addUnload(u_order);
    }
}

void MES::onStorageRequest(std::shared_ptr<std::string> response)
{
    std::stringstream ss;
    StorageDoc storeDoc(store);
    storeDoc.save(ss);
    *response = ss.str();
    MES_INFO("ERP: Storage request.");
}

void MES::onScheduleRequest(std::shared_ptr<std::string> response)
{
    std::stringstream ss;
    ScheduleDoc scheduleDoc(scheduler);
    scheduleDoc.save(ss);
    *response = ss.str();
    MES_INFO("ERP: Schedule request.");
}