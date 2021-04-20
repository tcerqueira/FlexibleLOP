#pragma once

#include "l_comms.h"
#include "Orders.h"
#include "Storage.h"

class Scheduler
{  
public:
    // note: takes ownership of the Order objects in the container
    void addOrderList(std::vector<TransformOrder*> &list);
    void addTransform(TransformOrder* order);
    void addUnload(UnloadOrder* order);
    // TransformOrder popOrder();
    std::vector<TransformOrder*> &getTransformOrders() { return t_orders; };
    std::vector<UnloadOrder*> &getUnloadOrders() { return u_orders; };

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Scheduler &sc);

private:
    std::vector<TransformOrder*> t_orders;
    std::vector<UnloadOrder*> u_orders;
    Storage *store;
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Scheduler &sc)
{
    os << std::endl << "Scheduler:" << std::endl;
    os << "== Transform Orders ==" << std::endl;
    for(int i=0; i < sc.t_orders.size(); i++)
    {
        os << i << " - " << *sc.t_orders[i] << std::endl;
    }

    os << "== Unload Orders ==" << std::endl;
    for(int i=0; i < sc.u_orders.size(); i++)
    {
        os << i << " - " << *sc.u_orders[i] << std::endl;
    }

    return os;
}
