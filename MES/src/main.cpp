#include <iostream>
#include <pugixml.hpp>
#include "XmlParser.h"
#include "Scheduler.h"
#include "LOProduction.h"
#include "Orders.h"

int main(int argc, char *argv[])
{
    std::cout << "Hello World!" << std::endl;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("test/OrderTest.xml");

    for(pugi::xml_node_iterator it = doc.begin(); it != doc.end(); ++it)
    {
        std::cout << "Order: ";
        for (pugi::xml_attribute_iterator ait = it->attributes_begin(); ait != it->attributes_end(); ++ait)
        {
            std::cout << " " << ait->name() << "=" << ait->value() << std::endl;
        }
    }

    OrderDoc orders;
    //std::cout << orders.getAmount() << std::endl;
    auto list = Order::CreateOrders(orders);
}
