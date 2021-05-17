#include "Scheduler.h"

void Scheduler::addOrderList(std::vector<std::shared_ptr<TransformOrder>> &list)
{
    for(std::shared_ptr<TransformOrder> order : list)
    {
        addTransform(order);
    }
}

void Scheduler::addTransform(std::shared_ptr<TransformOrder> order)
{
    // TODO: priority insertion
    const std::lock_guard<std::mutex> lock(transformVec_mutex);
    t1_orders.size() > t2_orders.size() ? t2_orders.push_back(order) : t1_orders.push_back(order);
    // t1_orders.push_back(order);
}

void Scheduler::addUnload(std::shared_ptr<UnloadOrder> order)
{
    const std::lock_guard<std::mutex> lock(unloadVec_mutex);
    u_orders.push_back(order);
}

void Scheduler::updatePieceStarted(int number)
{
    
}

void Scheduler::updatePieceFinished(int number)
{

}

bool Scheduler::hasTransform(int cell) const
{
    return cell == 1 ? !t1_orders.empty() : !t2_orders.empty();
}

bool Scheduler::hasUnload() const
{
    return !u_orders.empty();
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
