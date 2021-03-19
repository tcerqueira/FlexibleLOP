#include <iostream>
#include <memory>
#include "Orders.h"
#include "XmlParser.h"

Order::Order(int id, time_t receivedAt, int quantity)
: id(id), receivedAt(receivedAt), totalAmount(quantity)
{
}