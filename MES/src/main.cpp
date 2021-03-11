#include <iostream>
#include "pugixml.hpp"
#include "XmlParser.h"

int main(int argc, char *argv[])
{
    std::cout << "Hello World!" << std::endl;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("xgconsole.xml");
}
