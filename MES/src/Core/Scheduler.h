#pragma once

#include "l_comms.h"
#include "Orders/Orders.h"
#include "Factory/Storage.h"

#define WORK_TRANSFORM 8
#define WORK_UNLOAD 2
#define WORK_CHANGETOOLS 5

#define TOOLSET_BUFLEN 4
#define PIECESEQ_BUFLEN 8
#define PATH_BUFLEN 8
#define TOOLTIME_BUFLEN 8

struct SubOrder;

class Scheduler
{  
public:
    Scheduler(Storage *store);
    // note: takes ownership of the Order objects in the container
    void addOrderList(std::vector<std::shared_ptr<TransformOrder>> &list);
    void addTransform(std::shared_ptr<TransformOrder> order);
    void addUnload(std::shared_ptr<UnloadOrder> order);
    std::shared_ptr<UnloadOrder> requestUnload();
    std::shared_ptr<SubOrder> requestOrderCell(int cell);

    // Update events 
    void updatePieceStarted(int cell, int number);
    void updatePieceFinished(int cell, int number);
    // Estimated work
    int getCellWork(int cell) const;
    int getQueueWork(int cell) const;
    int getTotalWork(int cell) const;

    // get the transform order with id=number
    std::shared_ptr<TransformOrder> getTransform(int number);

    // Fetchs orders from the "to dispatch" list and sorts on priority
    void schedule();
    // Cleans orders if possible
    void clean();
    
    // TransformOrder popOrder();
    std::vector<std::shared_ptr<TransformOrder>> &getAllOrders() { return orders_list; };

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Scheduler &sc);

protected:
    struct OrderPriority {
        bool operator()(const std::shared_ptr<TransformOrder> o1, const std::shared_ptr<TransformOrder> o2) const;
        bool operator()(const std::shared_ptr<SubOrder> o1, const std::shared_ptr<SubOrder> o2) const;
    };
    
    std::shared_ptr<SubOrder> popSubOrder(int cell);

private:
    void priority_push_back(int cell, std::shared_ptr<SubOrder> sub_order);

private:
    std::mutex transforms_mutex, suborders_mutex, unloads_mutex;
    // all orders instances
    std::vector<std::shared_ptr<TransformOrder>> orders_list;
    // temporary orders to schedule
    std::vector<std::shared_ptr<TransformOrder>> to_dispatch;
    // sub orders for the cells
    std::list<std::shared_ptr<SubOrder>> cell1_orders;
    std::list<std::shared_ptr<SubOrder>> cell2_orders;
    // sub orders sent to the cells
    std::list<std::shared_ptr<SubOrder>> cell1_dispatched_orders;
    std::list<std::shared_ptr<SubOrder>> cell2_dispatched_orders;
    // queue of unload orders
    std::vector<std::shared_ptr<UnloadOrder>> unload_orders;
    // unload orders already dispatched
    std::vector<std::shared_ptr<UnloadOrder>> dispatched_unloads;

    Storage *store;
};

struct SubOrder
{
    int16_t orderID;
    uint16_t init_p;
    uint16_t final_p;
    int16_t quantity;
    int16_t to_do;
    int16_t done;
    std::vector<int16_t> tools;
    int16_t piece_seq[PIECESEQ_BUFLEN] = {0, 0, 0, 0, 0, 0, 0, 0};
    int16_t tool_set[TOOLSET_BUFLEN] = {0, 0, 0, 0};
    int16_t path[PATH_BUFLEN] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint64_t tool_time[TOOLTIME_BUFLEN] = {0, 0, 0, 0, 0, 0, 0, 0};
    bool warehouse_intermediate;
    uint16_t piece_intermediate;
    // necessary for calculating priority
    long priority;
    time_t readyTime;
    int work;
    int penalty;

    int calculateWork() {
        return to_do * (final_p-init_p);
    }

    long calculatePriority() {
        time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return (readyTime - now - work * WORK_TRANSFORM) * penalty; /// DAY_OF_WORK;
    }

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const SubOrder &so)
    {
        return os << "SubOrder [id=" << so.orderID << " Quantity=" << so.quantity << " ToDo=" << so.to_do << " Initial=" << so.init_p << " Work=" << so.work << " Penalty=" << so.penalty << "]";
    }
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Scheduler &sc)
{
    os << std::endl << "Scheduler:" << std::endl;
    // All transforms list
    os << "== ALL Transform Orders ==" << std::endl;
    for(int i=0; i < sc.orders_list.size(); i++)
    {
        os << i << " - " << *sc.orders_list[i] << std::endl;
    }
    // Cell 1 Orders
    os << "== Transform Orders C1 == " << std::endl;
    int j = 0;
    for(auto sub_order : sc.cell1_orders)
    {
        os << j++ << " - " << *sub_order << std::endl;
    }
    // Cell 2 Orders
    os << "== Transform Orders C2 == " << std::endl;
    j = 0;
    for(auto sub_order : sc.cell2_orders)
    {
        os << j++ << " - " << *sub_order << std::endl;
    }
#ifdef DEBUG
    // Dispatched Transforms
    os << "== Dispatched Orders C1 == " << std::endl;
    j = 0;
    for(auto sub_order : sc.cell1_dispatched_orders)
    {
        os << j++ << " - " << *sub_order << std::endl;
    }

    os << "== Dispatched Orders C2 == " << std::endl;
    j = 0;
    for(auto sub_order : sc.cell2_dispatched_orders)
    {
        os << j++ << " - " << *sub_order << std::endl;
    }
#endif
    // Unload Orders
    os << "== Unload Orders ==" << std::endl;
    for(int i=0; i < sc.unload_orders.size(); i++)
    {
        os << i << " - " << *sc.unload_orders[i] << std::endl;
    }
    // Dispatched unload orders
    os << "== Dispatched Unload Orders ==" << std::endl;
    for(int i=0; i < sc.dispatched_unloads.size(); i++)
    {
        os << i << " - " << *sc.dispatched_unloads[i] << std::endl;
    }
    // cell workload
    os << "== Cell Load ==" << std::endl;
    os << "Queue work: 1:" << sc.getQueueWork(1) << " 2:" << sc.getQueueWork(2) << std::endl;
    os << "Cell work: 1:" << sc.getCellWork(1) << " 2:" << sc.getCellWork(2) << std::endl;
    os << "Total work: 1:" << sc.getTotalWork(1) << " 2:" << sc.getTotalWork(2) << std::endl;

    return os;
}
