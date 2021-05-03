#pragma once

#include "l_comms.h"
#include <pugixml.hpp>
#include "Core/Storage.h"
#include "Core/Scheduler.h"

// Request node names
#define ROOT_NODE "ORDERS"
#define STORAGE_NODE "Request_Stores"
#define SCHEDULE_NODE "Request_Orders"
// Root node names for response
#define STORE_ROOT_NODE "Current_Stores"
#define SCHEDULE_ROOT_NODE "Order_Schedule"
#define STORE_PIECE_NODE "WorkPiece"
// Children node names
#define ORDER_NODE "Order"
#define TRANSF_NODE "Transform"
#define UNLOAD_NODE "Unload"

using namespace pugi;


class XmlRequest : public xml_document
{
public:
    xml_node root() const { return child(ROOT_NODE); }
    // NOT TESTED
    bool isLoaded() { return (bool) m_Result; }
    
private:
    xml_parse_result m_Result;
};

class OrderNode
{
public:
    OrderNode(xml_node order_root);
    // for transform ordfers
    const char* from() const;
    const char* to() const;
    int time() const;
    int maxdelay() const;
    int penalty() const;
    // for unload orders
    const char* type() const;
    const char* destination() const;
    // for both
    int number() const;
    int quantity() const;
    const char* name() const;
private:
    xml_node order_root;
};

class StorageDoc : public xml_document
{
public:
    StorageDoc(const Storage& store);

private:

};

class ScheduleDoc : public xml_document
{
public:
    ScheduleDoc(Scheduler& schedule);

private:

};