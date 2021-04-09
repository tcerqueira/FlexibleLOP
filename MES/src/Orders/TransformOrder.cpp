#include "Orders.h"

TransformOrder::TransformOrder(int id, time_t receivedAt, int quantity, piece_t initial, piece_t finalp, int penaltyPerDay, int maxSecDelay)
: Order(id, receivedAt, quantity), initial(initial), finalp(finalp), penaltyPerDay(penaltyPerDay), maxSecDelay(maxSecDelay), finalPenalty(0)
{
}
