#include "XmlParser.h"
#include "Orders/Orders.h"
#include "Utils.h"
#include <cstring>

StorageDoc::StorageDoc(const Storage& store)
{
    pugi::xml_node store_root = this->append_child(STORE_ROOT_NODE);
    pugi::xml_node workpiece_node;
    int count;
    for(int p=P1; p != End_p; p++)
    {
        count = store.countPiece(static_cast<piece_t>(p));
        char *str_piece = pieceToString(static_cast<piece_t>(p));
        workpiece_node = store_root.append_child(STORE_PIECE_NODE);

        workpiece_node.append_attribute("type") = str_piece;
        workpiece_node.append_attribute("quantity") = count;
        delete str_piece;
    }

}