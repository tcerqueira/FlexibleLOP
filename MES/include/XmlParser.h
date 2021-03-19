#pragma once

#include <string>
#include <pugixml.hpp>

using namespace pugi;

class XmlDoc : public xml_document
{
public:
    // load the xml file
    void load(const std::string &filename);
    // save document tree
    bool save(const std::string &filename);
    void save(std::ostream out);
    // NOT TESTED
    bool isLoaded() { return (bool) m_Result; }

protected:

private:
    xml_parse_result m_Result;
};

class OrderDoc : public XmlDoc
{
public:
    // amount getter
    int getAmount() const { return amount; }
    // for transform ordfers
    const char* from(int index);
    const char* to(int index);
    const char* time(int index);
    const char* maxdelay(int index);
    const char* penalty(int index);
    // for unload orders
    const char* type(int index);
    const char* destination(int index);
    // for both
    const char* quantity(int index);

private:
    int amount;
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