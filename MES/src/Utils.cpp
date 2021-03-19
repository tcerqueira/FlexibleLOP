#include "Utils.h"

piece_t parsePiece(const char* piece)
{
    auto str = std::string(piece);
    if(str == std::string("P1"))
        return P1;
    if(str == std::string("P2"))
        return P2;
    if(str == std::string("P3"))
        return P3;
    if(str == std::string("P4"))
        return P4;
    if(str == std::string("P5"))
        return P5;
    if(str == std::string("P6"))
        return P6;
    if(str == std::string("P7"))
        return P7;
    if(str == std::string("P8"))
        return P8;
    if(str == std::string("P9"))
        return P9;
}