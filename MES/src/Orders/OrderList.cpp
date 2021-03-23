#include "Orders.h"
#include "XmlParser.h"
#include <iostream>
#include <chrono>
#include "Utils.h"

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
            // std::cout << "Transform" << std::endl;

            order = new TransformOrder(orders.number(i), receivedAt, orders.quantity(i)
                , parsePiece(orders.from(i)), parsePiece(orders.to(i))
                , orders.penalty(i), orders.maxdelay(i));
            
        } 
        else if (it->child(UNLOAD_NODE))
        {
            // std::cout << "Unload" << std::endl;

            // TODO: UNLOAD CONSTRUCTOR
            order = new TransformOrder(orders.number(i), receivedAt, orders.quantity(i)
                , parsePiece(orders.from(i)), parsePiece(orders.to(i))
                , orders.penalty(i), orders.maxdelay(i));
        }
        else
        {
            std::cout << "null" << std::endl;
        }

        // TODO: ORDERED INSERTION
        if(order != nullptr)
            order_vec->push_back(order);
    }

    return order_vec;
}
