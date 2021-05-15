#pragma once

#include "l_comms.h"
#include <ctime>

class Order
{
public:
    Order(int id, time_t receivedAt, int quantity);
    virtual ~Order() = default;
    // getters
    int getId() const { return id; }
    int getDone() const { return doneAmount; }
    int getDoing() const { return doingAmount; }
    int getToDo() const { return totalAmount-doneAmount-doingAmount; }
    int getQuantity() const { return totalAmount; }
    time_t getTimeRcv() const { return receivedAt; }
    time_t getTimeSent() const { return sentAt; }
    time_t getTimeStarted() const { return startedAt; }
    time_t getTimeFinished() const { return finishedAt; }

    void pieceDone();
    void pieceDoing();
    void sent();
    void started();
    void finished();
    
    // virtual int getType() const { return 0; };

    Order &operator--();
    Order &operator--(int);

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Order &o);

protected:
    int id;
    int doneAmount;
    int doingAmount;
    int totalAmount;
    time_t receivedAt;
    time_t sentAt;
    time_t startedAt;
    time_t finishedAt;
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Order &o)
{
    return os << "Order [id=" << o.id << " Quantity=" << o.totalAmount << " Time=" << o.receivedAt << "]";
}
