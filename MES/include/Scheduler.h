#pragma once

#include "l_comms.h"
#include "Orders.h"
#include "Storage.h"

class Scheduler
{  
public:
    // note: takes ownership of the Order objects in the container
    void addOrderList(std::vector<Order*> &list);
    void addOrder(Order* order);
    Order popOrder();

private:
    std::vector<Order*> orders;
    Storage *store;
};
