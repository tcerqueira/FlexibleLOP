#include "XmlParser.h"

OrderNode::OrderNode(pugi::xml_node order_root)
: order_root(order_root)
{    
}

const char* OrderNode::from() const
{
    return order_root.child(TRANSF_NODE).attribute("From").as_string();
}

const char* OrderNode::to() const
{
    return order_root.child(TRANSF_NODE).attribute("To").as_string();
}

int OrderNode::time() const
{
    return order_root.child(TRANSF_NODE).attribute("Time").as_int();
}

int OrderNode::maxdelay() const
{
    return order_root.child(TRANSF_NODE).attribute("MaxDelay").as_int();
}

int OrderNode::penalty() const
{
    return order_root.child(TRANSF_NODE).attribute("Penalty").as_int();
}

// UNLOAD TYPE OF ORDER

const char* OrderNode::type() const
{
    return order_root.child(UNLOAD_NODE).attribute("Type").as_string();
}

const char* OrderNode::destination() const
{
    return order_root.child(UNLOAD_NODE).attribute("Destination").as_string();
}

// BOTH TYPES OF ORDERS

int OrderNode::number() const
{
    return order_root.attribute("Number").as_int();
}

int OrderNode::quantity() const
{
    if(order_root.child(TRANSF_NODE))
        return order_root.child(TRANSF_NODE).attribute("Quantity").as_int();

    return order_root.child(UNLOAD_NODE).attribute("Quantity").as_int();
}

const char* OrderNode::name() const
{
    return order_root.first_child().name();
}
