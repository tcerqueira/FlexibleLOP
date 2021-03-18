#pragma once

#include <vector>
#include <ctime>
#include <memory>
#include "XmlParser.h"

class Order
{
public:
    // getter to id
    int getId() { return id; }
    // factory method
    static std::shared_ptr<std::vector<Order*>> CreateOrders(const OrderDoc& orders);

private:
    int id;
    time_t receivedAt;
    time_t sentAt;
    time_t startedAt;
    time_t finishedAt;
};

class OrderList
{
public:
    // adds Order
    void add(Order* order);
    void add(Order* order, int index);
    // removes Order and returns the object
    Order* remove(int index);

private:
    std::vector<Order*> list;
};

enum piece_t { P1 = 1, P2, P3, P4, P5, P6, P7, P8, P9};
enum dest_t { PM1 = 1, PM2, PM3 };

class TransformOrder : public Order
{
public:
private:
    piece_t initial;
    piece_t final;
    int doneAmount = 0;
    int doingAmount = 0;
    int totalAmount;
    int penaltyPerDay;
    int maxSecDelay;
    int finalPenalty;
};

class UnloadOrder : public Order
{
public:
private:
    piece_t piece;
    dest_t dest;
    int doneAmount = 0;
    int doingAmount = 0;
    int totalAmount;
};
