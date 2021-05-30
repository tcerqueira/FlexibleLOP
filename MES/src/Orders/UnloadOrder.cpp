#include "UnloadOrder.h"

UnloadOrder::UnloadOrder(int id, time_t receivedAt, int quantity, piece_t piece, dest_t dest)
: Order(id, receivedAt, quantity), piece(piece), dest(dest)
{
}
