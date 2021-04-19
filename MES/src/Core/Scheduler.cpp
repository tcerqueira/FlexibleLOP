#include "Scheduler.h"

void Scheduler::addOrderList(std::vector<TransformOrder*> &list)
{
    for(TransformOrder* order : list)
    {
        addTransform(order);
    }
}

void Scheduler::addTransform(TransformOrder* order)
{
    // TODO: priority insertion
    t_orders.push_back(order);
}

void Scheduler::addUnload(UnloadOrder* order)
{
    u_orders.push_back(order);
}

// TransformOrder Scheduler::popOrder()
// {
//     TransformOrder order = --(*t_orders.front());
//     // if all orders dispatched
//     if(order.getDoing() + order.getDone() >= order.getQuantity()){
//         MES_TRACE("Order {} dispatched.", order);
//         // TODO: add it to dispatched list
//     }

//     return order;
// }

TransformOrder* Scheduler::orderAt(int index) const
{
    return t_orders[index];
}