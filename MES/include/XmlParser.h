#pragma once

#include <string>
#include <pugixml.hpp>

#define ROOT_NODE "ORDERS"
#define TRANSF_NODE "Transform"
#define UNLOAD_NODE "Unload"

using namespace pugi;


class XmlDoc : public xml_document
{
public:
    xml_node root() const { return child(ROOT_NODE); }
    // NOT TESTED
    bool isLoaded() { return (bool) m_Result; }

protected:

private:
    xml_parse_result m_Result;
};

class OrderDoc : public XmlDoc
{
public:
    // for transform ordfers
    const char* from(int index) const;
    const char* to(int index) const;
    int time(int index) const;
    int maxdelay(int index) const;
    int penalty(int index) const;
    // for unload orders
    const char* type(int index) const;
    const char* destination(int index) const;
    // for both
    int number(int index) const;
    int quantity(int index) const;

private:
    xml_node orderAt(int index) const;
};

class StorageDoc : public XmlDoc
{
public:

private:

};

class ScheduleDoc : public XmlDoc
{
public:

private:

};