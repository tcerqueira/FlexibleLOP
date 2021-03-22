#include "XmlParser.h"

xml_node OrderDoc::orderAt(int index) const
{
    xml_node it = this->root().first_child();
    for(int i=0; i < index; i++ && it, it = it.next_sibling())
    { }

    return it;
}

// TRANSFORM TYPE OF ORDER

const char* OrderDoc::from(int index) const
{
    return orderAt(index).child(TRANSF_NODE).attribute("From").as_string();
}

const char* OrderDoc::to(int index) const
{
    return orderAt(index).child(TRANSF_NODE).attribute("To").as_string();
}

int OrderDoc::time(int index) const
{
    return orderAt(index).child(TRANSF_NODE).attribute("Time").as_int();
}

int OrderDoc::maxdelay(int index) const
{
    return orderAt(index).child(TRANSF_NODE).attribute("MaxDelay").as_int();
}

int OrderDoc::penalty(int index) const
{
    return orderAt(index).child(TRANSF_NODE).attribute("Penalty").as_int();
}

// UNLOAD TYPE OF ORDER

const char* OrderDoc::type(int index) const
{
    return orderAt(index).child(UNLOAD_NODE).attribute("Type").as_string();
}

const char* OrderDoc::destination(int index) const
{
    return orderAt(index).child(UNLOAD_NODE).attribute("Destination").as_string();
}

// BOTH TYPES OF ORDERS

int OrderDoc::number(int index) const
{
    return orderAt(index).attribute("Number").as_int();
}

int OrderDoc::quantity(int index) const
{
    if(orderAt(index).child(TRANSF_NODE))
        return orderAt(index).child("Transform").attribute("Quantity").as_int();

    return orderAt(index).child(UNLOAD_NODE).attribute("Quantity").as_int();
}
