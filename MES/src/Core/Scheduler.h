#pragma once

#include "l_comms.h"
#include "Orders/Orders.h"
#include "Factory/Storage.h"

#define WORK_TRANSFORM 30
#define WORK_UNLOAD 15
#define WORK_CHANGETOOLS 20

struct SubOrder
{
    int16_t orderID;
    uint16_t init_p;
    int16_t quantity;
    int16_t to_do;
    int16_t done;
    int16_t tool_set[4] = {0, 0, 0, 0};
    int16_t path[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint64_t tool_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    bool warehouse_intermediate;
    uint16_t piece_intermediate;
    // necessary for calculating priority
    time_t readyTime;
    int work;
    int penalty;

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const SubOrder &so)
    {
        return os << "SubOrder [id=" << so.orderID << " Quantity=" << so.quantity << " ToDo=" << so.to_do << " Initial=" << so.init_p << " Work=" << so.work << " Penalty=" << so.penalty << "]";
    }
};

class Scheduler
{  
public:
    Scheduler(Storage *store);
    // note: takes ownership of the Order objects in the container
    void addOrderList(std::vector<std::shared_ptr<TransformOrder>> &list);
    void addTransform(std::shared_ptr<TransformOrder> order);
    void addUnload(std::shared_ptr<UnloadOrder> order);
    std::shared_ptr<UnloadOrder> popUnload();
    std::shared_ptr<SubOrder> popOrderCell(int cell);
    bool hasTransform(int cell) const;
    bool hasUnload() const;

    void updatePieceStarted(int cell, int number);
    void updatePieceFinished(int cell, int number);
    int getTotalWork(int cell);

    std::shared_ptr<TransformOrder> getTransform(int number);

    void schedule();
    
    // TransformOrder popOrder();
    std::vector<std::shared_ptr<TransformOrder>> &getAllOrders() { return orders_list; };
    std::vector<std::shared_ptr<SubOrder>> &getTransformOrdersC1() { return t1_orders; };
    std::vector<std::shared_ptr<SubOrder>> &getTransformOrdersC2() { return t2_orders; };
    std::vector<std::shared_ptr<UnloadOrder>> &getUnloadOrders() { return u_orders; };

    struct OrderPriority {
        bool operator()(const std::shared_ptr<TransformOrder> o1, const std::shared_ptr<TransformOrder> o2) const;
        bool operator()(const std::shared_ptr<SubOrder> o1, const std::shared_ptr<SubOrder> o2) const;
    };

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Scheduler &sc);

private:
    std::mutex transformVec_mutex;
    std::mutex unloadVec_mutex;
    // all orders instances
    std::vector<std::shared_ptr<TransformOrder>> orders_list;
    // temporary orders to schedule
    std::vector<std::shared_ptr<TransformOrder>> to_dispatch;
    // sub orders for the cells
    std::vector<std::shared_ptr<SubOrder>> t1_orders;
    std::vector<std::shared_ptr<SubOrder>> t2_orders;
    // queue of unload orders TODO: change to queue data structure
    std::vector<std::shared_ptr<UnloadOrder>> u_orders;
    // orders already dispatched by the factory
    std::vector<std::shared_ptr<TransformOrder>> dispatched_transforms;
    std::vector<std::shared_ptr<UnloadOrder>> dispatched_unloads;

    Storage *store;
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Scheduler &sc)
{
    // std::vector<std::shared_ptr<SubOrder>> t1_tmp(sc.t1_orders);
    // std::vector<std::shared_ptr<SubOrder>> t2_tmp(sc.t2_orders);

    // std::sort_heap(t1_tmp.begin(), t1_tmp.end(), Scheduler::OrderPriority());
    // std::sort_heap(t2_tmp.begin(), t2_tmp.end(), Scheduler::OrderPriority());

    os << std::endl << "Scheduler:" << std::endl;
    os << "== ALL Transform Orders ==" << std::endl;
    for(int i=0; i < sc.orders_list.size(); i++)
    {
        os << i << " - " << *sc.orders_list[i] << std::endl;
    }

    os << "== Transform Orders C1 == " << std::endl;
    int j = 0;
    for(auto sub_order : sc.t1_orders)
    {
        os << j++ << " - " << *sub_order << std::endl;
    }

    os << "== Transform Orders C2 == " << std::endl;
    j = 0;
    for(auto sub_order : sc.t2_orders)
    {
        os << j++ << " - " << *sub_order << std::endl;
    }

    os << "== Unload Orders ==" << std::endl;
    for(int i=0; i < sc.u_orders.size(); i++)
    {
        os << i << " - " << *sc.u_orders[i] << std::endl;
    }

    return os;
}
