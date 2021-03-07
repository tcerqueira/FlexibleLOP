#include <iostream>
#include "../libs/pugixml-1.11/src/pugixml.hpp"

int main(int argc, char *argv[])
{
    std::cout << "Hello World!" << std::endl;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("xgconsole.xml");
}
