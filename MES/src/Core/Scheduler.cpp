#include "Scheduler.h"
#include <algorithm>

std::shared_ptr<SubOrder> toSubOrder(const std::shared_ptr<TransformOrder> order);

Scheduler::Scheduler()
{
    std::make_heap(to_dispatch.begin(), to_dispatch.end(), OrderPriority());
    std::make_heap(t1_orders.begin(), t1_orders.end(), OrderPriority());
    std::make_heap(t2_orders.begin(), t2_orders.end(), OrderPriority());
}

void Scheduler::addOrderList(std::vector<std::shared_ptr<TransformOrder>> &list)
{
    for (std::shared_ptr<TransformOrder> order : list)
    {
        addTransform(order);
    }
}

void Scheduler::addTransform(std::shared_ptr<TransformOrder> order)
{
    // TODO: priority insertion
    const std::lock_guard<std::mutex> lock(transformVec_mutex);
    orders_list.push_back(order);
    to_dispatch.push_back(order);
    std::push_heap(to_dispatch.begin(), to_dispatch.end(), OrderPriority());
}

void Scheduler::addUnload(std::shared_ptr<UnloadOrder> order)
{
    const std::lock_guard<std::mutex> lock(unloadVec_mutex);
    u_orders.push_back(order);
}

std::shared_ptr<UnloadOrder> Scheduler::popUnload()
{
    int i = 0;
    for(auto unload : u_orders)
    {
        if(unload->getQuantity() <= store->countPiece(unload->getPiece()))
        {
            dispatched_unloads.push_back(unload);
            u_orders.erase(u_orders.begin()+i);
            return unload;
        }
        i++;
    }
    
    return nullptr;
}

void Scheduler::schedule()
{
    while(to_dispatch.size() > 0)
    {
        std::pop_heap(to_dispatch.begin(), to_dispatch.end());
        auto order = to_dispatch.back();
        to_dispatch.pop_back();

        auto sub_order = toSubOrder(order);

        if (getTotalWork(1) <= getTotalWork(2))
        {
            t1_orders.push_back(order);
            std::push_heap(t1_orders.begin(), t1_orders.end(), OrderPriority());
        }
        else
        {
            t2_orders.push_back(order);
            std::push_heap(t2_orders.begin(), t2_orders.end(), OrderPriority());
        }
    }
    MES_TRACE("Work cell 1:{}  Work cell 2:{}", getTotalWork(1), getTotalWork(2));
    to_dispatch.clear();
}

int Scheduler::getTotalWork(int cell)
{
    int work = 0;
    if (cell == 1)
    {
        for (auto order : t1_orders)
            work += WORK_TRANSFORM * (order->getEstimatedWork());
        work += WORK_CHANGETOOLS * t1_orders.size();
    }
    else
    {
        for (auto order : t2_orders)
            work += WORK_TRANSFORM * (order->getEstimatedWork());
        for(auto unload : u_orders)
            work += WORK_UNLOAD * unload->getQuantity();
        work += WORK_CHANGETOOLS * t2_orders.size();
    }

    return work;
}

bool Scheduler::OrderPriority::operator()(const std::shared_ptr<TransformOrder> o1, const std::shared_ptr<TransformOrder> o2) const
{
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    long p1 = (o1->getReadyTime() - now - o1->getEstimatedWork() * WORK_TRANSFORM) * o1->getDailyPenalty();
    long p2 = (o2->getReadyTime() - now - o2->getEstimatedWork() * WORK_TRANSFORM) * o2->getDailyPenalty();

    return p1 > p2;
}

bool Scheduler::OrderPriority::operator()(const std::shared_ptr<SubOrder> o1, const std::shared_ptr<SubOrder> o2) const
{
    return true;
}

void Scheduler::updatePieceStarted(int cell, int number)
{
    // store->subCount(piece, 1);
}

void Scheduler::updatePieceFinished(int cell, int number)
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

// ########################### AUXILIAR FUNCTIONS ####################################
// ###################################################################################

std::shared_ptr<SubOrder> toSubOrder(const std::shared_ptr<TransformOrder> order)
{
    auto sub_order = std::make_shared<SubOrder>();
    sub_order->orderID = (int16_t)order->getId();
    sub_order->init_p = (uint16_t)order->getInitial();
    sub_order->quantity = (int16_t)order->getQuantity();
    sub_order->to_do = (int16_t)order->getToDo();
    sub_order->done = (int16_t)order->getDone();
    // sub_ortder->tool_set = ;
    // sub_order->path = ;
    // sub_order->tool_time = ;
    // sub_order->warehouse_intermediate = ;
    // sub_order->piece_intermediate = ;
    sub_order->readyTime = order->getReadyTime();
    sub_order->work = order->getEstimatedWork();
    sub_order->penalty = order->getDailyPenalty();

    return sub_order;
}
