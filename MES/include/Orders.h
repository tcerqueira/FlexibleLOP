#pragma once

#include <vector>

class Order
{
public:
    // getter to id

private:
    int id;
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