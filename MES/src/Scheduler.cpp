#include "Scheduler.h"

void Scheduler::addOrderList(std::vector<Order*> &list)
{
    for(Order* order : list)
    {
        addOrder(order);
    }
}

void Scheduler::addOrder(Order* order)
{
    // TODO: priority insertion
    orders.push_back(order);
}

Order Scheduler::popOrder()
{
    Order order = --(*orders.front());
    // if all orders dispatched
    if(order.getDoing() + order.getDone() >= order.getQuantity()){
        MES_TRACE("Order {} dispatched.", order.getId());
        // TODO: add it to dispatched list
    }

    return order;
}