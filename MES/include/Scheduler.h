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
    Order* orderAt(int index) const;
    int orderSize() const { return orders.size(); }

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Scheduler &sc);

private:
    std::vector<Order*> orders;
    Storage *store;
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Scheduler &sc)
{
    for(int i=0; i < sc.orders.size(); i++)
    {
        os << i << " - " << *sc.orders[i] << std::endl;
    }

    return os;
}
