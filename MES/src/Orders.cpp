#include <iostream>
#include <memory>
#include "Orders.h"
#include "XmlParser.h"


std::shared_ptr<std::vector<Order*>> Order::CreateOrders(const OrderDoc& orders)
{
    std::shared_ptr<std::vector<Order*>> order_vec = std::make_shared<std::vector<Order*>>();
    //std::vector<Order*> order_vec;
    std::cout << order_vec->size() << std::endl;

    return order_vec;
}