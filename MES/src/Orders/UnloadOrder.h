#pragma once

#include <ctime>
#include "Order.h"
#include "Order_enums.h"

class UnloadOrder : public Order
{
public:
    UnloadOrder(int id, time_t receivedAt, int quantity, piece_t piece, dest_t dest);
    // getters
    piece_t getPiece() const { return piece; }
    dest_t getDest() const { return dest; }

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const UnloadOrder &o);

private:
    piece_t piece;
    dest_t dest;
};

// STREAM OVERLOADS
template <typename OStream>
OStream &operator<<(OStream &os, const UnloadOrder &o)
{
    return os << "UnloadOrder [id=" << o.getId() << " Quantity=" << o.getQuantity() << " Time=" << o.getTimeRcv() << " Piece="<< (int)o.piece << " Dest=" << (int)o.dest << "]";
}
