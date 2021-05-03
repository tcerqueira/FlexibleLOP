#include "Order.h"

Order::Order(int id, time_t receivedAt, int quantity)
    : id(id), receivedAt(receivedAt), totalAmount(quantity), doneAmount(0), doingAmount(0)
{
}

void Order::pieceDone()
{
    doneAmount++;
}

void Order::pieceDoing()
{
    doingAmount++;
}

void Order::sent()
{
    sentAt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

void Order::started()
{
    startedAt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

void Order::finished()
{
    finishedAt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}


// OPERATOR ORVERLOADS

Order &Order::operator--()
{
    pieceDoing();
    return *this;
}

Order &Order::operator--(int)
{
    pieceDoing();
    return *this;
}
