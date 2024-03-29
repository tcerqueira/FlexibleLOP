#include "Utils.h"
#include "cstring"

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
    
    return End_p;
}

dest_t parseDest(const char* dest)
{
    auto str = std::string(dest);
    if(str == std::string("PM1"))
        return PM1;
    if(str == std::string("PM2"))
        return PM2;
    if(str == std::string("PM3"))
        return PM3;
    
    return End_d;
}

char* pieceToString(piece_t piece)
{
    char* str =  new char[3];
    str[0] = '\0';
    switch (piece)
    {
    case P1: strcpy(str, "P1"); break;
    case P2: strcpy(str, "P2"); break;
    case P3: strcpy(str, "P3"); break;
    case P4: strcpy(str, "P4"); break;
    case P5: strcpy(str, "P5"); break;
    case P6: strcpy(str, "P6"); break;
    case P7: strcpy(str, "P7"); break;
    case P8: strcpy(str, "P8"); break;
    case P9: strcpy(str, "P9"); break;
    }
    return str;
}

char* destToString(dest_t dest)
{
    char* str =  new char[4];
    str[0] = '\0';
    switch (dest)
    {
    case PM1: strcpy(str, "PM1"); break;
    case PM2: strcpy(str, "PM2"); break;
    case PM3: strcpy(str, "PM3"); break;
    }
    return str;
}
