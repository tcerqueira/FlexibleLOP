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
    const std::lock_guard<std::mutex> lock(transformVec_mutex);
    // t1_orders.size() > t2_orders.size() ? t2_orders.push_back(order) : t1_orders.push_back(order);
    t1_orders.push_back(order);
}

void Scheduler::addUnload(UnloadOrder* order)
{
    const std::lock_guard<std::mutex> lock(unloadVec_mutex);
    u_orders.push_back(order);
}

// TransformOrder Scheduler::popOrder()
// {
//     TransformOrder order = --(*t1_orders.front());
//     // if all orders dispatched
//     if(order.getDoing() + order.getDone() >= order.getQuantity()){
//         MES_TRACE("Order {} dispatched.", order);
//         // TODO: add it to dispatched list
//     }

//     return order;
// }
