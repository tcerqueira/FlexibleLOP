#include "Orders.h"
#include "Utils.h"

Order::Order(int id, time_t receivedAt, int quantity)
    : id(id), receivedAt(receivedAt), totalAmount(quantity), doneAmount(0), doingAmount(0)
{
}

Order *Order::Factory(const OrderNode &order_node)
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
    }

    return order;
}

// OPERATOR ORVERLOADS

Order &Order::operator--()
{
    doingAmount++;
    return *this;
}

Order &Order::operator--(int)
{
    doingAmount++;
    return *this;
}

// template <typename OStream>
// OStream& operator<<(OStream &os, const Order &o)
// {
//     return os << "[Order id=" << o.id << " Quantity=" << o.totalAmount << "Time=" << o.receivedAt << "]";
// }
