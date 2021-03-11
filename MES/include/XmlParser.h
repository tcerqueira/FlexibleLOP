#pragma once

#include <string>
#include "pugixml.hpp"

using namespace pugi;

class XmlDoc
{
public:
    // load the xml file
    void load(const std::string &filename);
    // save document tree
    bool save(const std::string &filename);
    void save(std::ostream out);

protected:
    // document getter
    xml_document* getDocument() { return &m_Doc; }

private:
    xml_document m_Doc;
    xml_parse_result m_Result;
};

class OrderDoc : public XmlDoc
{
public:

private:

};