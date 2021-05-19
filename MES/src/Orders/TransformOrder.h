#pragma once

#include <ctime>
#include "Order.h"
#include "Order_enums.h"

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

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const TransformOrder &o)
{
    return os << "TransformOrder [id=" << o.getId() << " Quantity=" << o.getQuantity() << " Time=" << o.getTimeRcv() << " Piece_I="<< (int)o.initial << " Piece_F="<< (int)o.finalp << " Penalty=" << o.penaltyPerDay << "]";
}
