#pragma once

#include "l_comms.h"
#include <ctime>
#include "XmlParser.h"

class OrderList;

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

    Order& operator--();
    Order& operator--(int);

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

class OrderList : public std::vector<Order*>
{
public:
    // adds Order
    void add(Order* order);
    void add(Order* order, int index);
    // removes Order and returns the object
    Order* remove(int index);
    // factory method
    static OrderList* CreateOrders(const OrderDoc& orders);

private:
};

enum piece_t { P1 = 1, P2, P3, P4, P5, P6, P7, P8, P9};
enum dest_t { PM1 = 1, PM2, PM3 };

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
