#include "Orders.h"
#include "XmlParser.h"
#include <chrono>
#include "Utils.hpp"

OrderList* OrderList::CreateOrders(const OrderDoc& orders)
{
    // create empty list
    OrderList* order_vec = new OrderList();

    // time it was received
    time_t receivedAt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // loop for every order in xml document
    int i = 0;
    for (pugi::xml_node_iterator it = orders.root().begin(); it != orders.root().end(); it++, i++)
    {
        Order* order = nullptr;
        if(it->child(TRANSF_NODE))
        {
            order = new TransformOrder(orders.number(i), receivedAt, orders.quantity(i)
                , parsePiece(orders.from(i)), parsePiece(orders.to(i))
                , orders.penalty(i), orders.maxdelay(i));
        } 
        else if (it->child(UNLOAD_NODE))
        {
            // TODO: UNLOAD CONSTRUCTOR
            order = new UnloadOrder(orders.number(i), receivedAt, orders.quantity(i)
                , parsePiece(orders.type(i)), parseDest(orders.destination(i)));
        }
        else
        {
            MES_WARN("Order {} - type {} undefined.", orders.number(i), it->first_child().name());
        }

        // TODO: ORDERED INSERTION
        if(order != nullptr)
            order_vec->push_back(order);
    }

    return order_vec;
}
