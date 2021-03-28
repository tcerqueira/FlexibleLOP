#pragma once
#include <string>
#include "Orders.h"

piece_t parsePiece(const char* piece);

dest_t parseDest(const char*  dest);

char* pieceToString(piece_t piece);

const char* destToString(dest_t dest);