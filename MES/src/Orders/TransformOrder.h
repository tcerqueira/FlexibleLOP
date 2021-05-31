#pragma once

#include <ctime>
#include "Order.h"
#include "Order_enums.h"

#define DAY_OF_WORK 50

class TransformOrder : public Order
{
public:
    TransformOrder(int id, time_t receivedAt, int quantity, piece_t initial, piece_t finalp, int penaltyPerDay, int maxSecDelay);
    // getters
    piece_t getInitial() const { return initial; }
    piece_t getFinal() const { return finalp; }
    int getDailyPenalty() const { return penaltyPerDay; }
    int getMaxDelay() const { return maxSecDelay; }
    int getPenalty() const;
    time_t getReadyTime() const;
    int getEstimatedWork() const;

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const TransformOrder &o);

private:
    piece_t initial;
    piece_t finalp;
    int penaltyPerDay;
    int maxSecDelay;
};

int calculatePenalty(int penaltyPerDay, time_t finishedAt, time_t sentAt, int maxSecDelay, int secondsPerDay);

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const TransformOrder &o)
{
    std::string status;
    if(o.doneAmount == o.totalAmount)
        status = std::move("(status=Done)");
    else if(o.doingAmount > 0)
        status = std::move("(status=Doing)");
    else
        status = std::move("(status=Not started)");
    
    return os << "TransformOrder [id=" << o.getId() << " Quantity=" << o.getQuantity() << " Time=" << o.getTimeRcv() << " Piece_I="<< (int)o.initial << " Piece_F="<< (int)o.finalp << " Penalty=" << o.penaltyPerDay << "]";
}
