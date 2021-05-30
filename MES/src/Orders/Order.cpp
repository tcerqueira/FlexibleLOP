#include "Order.h"

static std::mutex mtx;

Order::Order(int id, time_t receivedAt, int quantity)
    : id(id), receivedAt(receivedAt), totalAmount(quantity), doneAmount(0), doingAmount(0)
{
}

void Order::pieceDone()
{
    std::lock_guard<std::mutex> lck(mtx);
    doneAmount++;
    doingAmount--;
}

void Order::pieceDoing()
{
    std::lock_guard<std::mutex> lck(mtx);
    doingAmount++;
}

void Order::sent()
{
    std::lock_guard<std::mutex> lck(mtx);
    sentAt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

void Order::started()
{
    std::lock_guard<std::mutex> lck(mtx);
    startedAt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

void Order::finished()
{
    std::lock_guard<std::mutex> lck(mtx);
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
