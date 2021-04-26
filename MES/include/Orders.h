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
    
    virtual int getType() const { return 0; };

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

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const TransformOrder &o);

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

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const UnloadOrder &o);

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

template <typename OStream>
OStream &operator<<(OStream &os, const TransformOrder &o)
{
    return os << "[TransformOrder id=" << o.getId() << " Quantity=" << o.getQuantity() << " Time=" << o.getTimeRcv() << " Piece_I="<< (int)o.initial << " Piece_F="<< (int)o.finalp << " Penalty=" << o.penaltyPerDay << "]";
}

template <typename OStream>
OStream &operator<<(OStream &os, const UnloadOrder &o)
{
    return os << "[UnloadOrder id=" << o.getId() << " Quantity=" << o.getQuantity() << " Time=" << o.getTimeRcv() << " Piece="<< (int)o.piece << " Dest=" << (int)o.dest << "]";
}