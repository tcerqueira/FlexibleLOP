#include "Order.h"

Order::Order(int id, time_t receivedAt, int quantity)
    : id(id), receivedAt(receivedAt), totalAmount(quantity), doneAmount(0), doingAmount(0)
{
}

// OPERATOR ORVERLOADS

Order &Order::operator--()
{
    doingAmount++;
    return *this;
}

Order &Order::operator--(int)
{
    doingAmount++;
    return *this;
}
