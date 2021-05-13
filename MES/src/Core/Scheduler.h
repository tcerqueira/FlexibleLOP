#pragma once

#include "l_comms.h"
#include "Orders/Orders.h"
#include "Storage.h"

class Scheduler
{  
public:
    // note: takes ownership of the Order objects in the container
    void addOrderList(std::vector<std::shared_ptr<TransformOrder>> &list);
    void addTransform(std::shared_ptr<TransformOrder> order);
    void addUnload(std::shared_ptr<UnloadOrder> order);
    // TransformOrder popOrder();
    std::vector<std::shared_ptr<TransformOrder>> &getTransformOrdersC1() { return t1_orders; };
    std::vector<std::shared_ptr<TransformOrder>> &getTransformOrdersC2() { return t2_orders; };
    std::vector<std::shared_ptr<UnloadOrder>> &getUnloadOrders() { return u_orders; };

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Scheduler &sc);

private:
    std::mutex transformVec_mutex;
    std::mutex unloadVec_mutex;
    std::vector<std::shared_ptr<TransformOrder>> t1_orders;
    std::vector<std::shared_ptr<TransformOrder>> t2_orders;
    std::vector<std::shared_ptr<UnloadOrder>> u_orders;
    std::vector<std::shared_ptr<TransformOrder>> transform_done;
    std::vector<std::shared_ptr<UnloadOrder>> unload_done;
    Storage *store;
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Scheduler &sc)
{
    os << std::endl << "Scheduler:" << std::endl;
    os << "== Transform Orders C1 ==" << std::endl;
    for(int i=0; i < sc.t1_orders.size(); i++)
    {
        os << i << " - " << *sc.t1_orders[i] << std::endl;
    }

    os << "== Transform Orders C2 ==" << std::endl;
    for(int i=0; i < sc.t2_orders.size(); i++)
    {
        os << i << " - " << *sc.t2_orders[i] << std::endl;
    }

    os << "== Unload Orders ==" << std::endl;
    for(int i=0; i < sc.u_orders.size(); i++)
    {
        os << i << " - " << *sc.u_orders[i] << std::endl;
    }

    return os;
}
