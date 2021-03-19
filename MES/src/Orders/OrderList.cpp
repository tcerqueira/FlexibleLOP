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
    for (pugi::xml_node_iterator it = orders.begin(); it != orders.end(); ++it)
    {
        Order* order = nullptr;
        if(!it->child("Transform"))
        {
            std::cout << "Transform" << std::endl;
            std::cout << parsePiece("P2") << std::endl; 
            // pugi::xml_node transform = it->child("Tranform");

            // order = new TransformOrder(it->attribute("Number").as_int(), receivedAt, transform.attribute("Quantity")
            //     , parsePiece(transform.attribute("From").as_string())
            //     , );

        } 
        else if (!it->child("Unload"))
        {
            std::cout << "Unload" << std::endl;
            // order = new UnloadOrder(it->attribute("Number").as_int(), receivedAt
            //     , );
        }
        else
        {
            std::cout << "null" << std::endl;
        }

        if(order != nullptr)
            order_vec->push_back(order);
    }
    //std::vector<Order*> order_vec;
    std::cout << order_vec->size() << std::endl;

    return order_vec;
}
