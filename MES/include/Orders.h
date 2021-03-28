#pragma once

#include "l_comms.h"
#include <ctime>

class Order
{
public:
    Order(int id, time_t receivedAt, int quantity);
    // getters
    int getId() const { return id; }
    int getDone() const { return doneAmount; }
    int getDoing() const { return doingAmount; }
    int getQuantity() const { return totalAmount; }
    int getTimeRcv() const { return receivedAt; }
    int getTimeSent() const { return sentAt; }
    int getTimeStarted() const { return startedAt; }
    int getTimeFinished() const { return finishedAt; }

    Order &operator--();
    Order &operator--(int);

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Order &o);

private:
    int id;
    int doneAmount;
    int doingAmount;
    int totalAmount;
    time_t receivedAt;
    time_t sentAt;
    time_t startedAt;
    time_t finishedAt;
};

enum piece_t
{
    P1 = 1, P2, P3, P4, P5, P6, P7, P8, P9, End_p
};
enum dest_t
{
    PM1 = 1, PM2, PM3, End_d
};

class TransformOrder : public Order
{
public:
    TransformOrder(int id, time_t receivedAt, int quantity, piece_t initial, piece_t finalp, int penaltyPerDay, int maxSecDelay);
    // getters
    piece_t getInitial() const { return initial; }
    piece_t getFinal() const { return finalp; }
    int getDailyPenalty() const { return penaltyPerDay; }
    int getMaxDelay() const { return maxSecDelay; }
    int getPenalty() const { return finalPenalty; }

private:
    piece_t initial;
    piece_t finalp;
    int penaltyPerDay;
    int maxSecDelay;
    int finalPenalty;
};

class UnloadOrder : public Order
{
public:
    UnloadOrder(int id, time_t receivedAt, int quantity, piece_t piece, dest_t dest);
    // getters
    piece_t getPiece() const { return piece; }
    dest_t getDest() const { return dest; }

private:
    piece_t piece;
    dest_t dest;
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const Order &o)
{
    return os << "[Order id=" << o.id << " Quantity=" << o.totalAmount << " Time=" << o.receivedAt << "]";
}
