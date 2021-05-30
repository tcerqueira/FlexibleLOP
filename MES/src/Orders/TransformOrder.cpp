#include "TransformOrder.h"

TransformOrder::TransformOrder(int id, time_t receivedAt, int quantity, piece_t initial, piece_t finalp, int penaltyPerDay, int maxSecDelay)
: Order(id, receivedAt, quantity), initial(initial), finalp(finalp), penaltyPerDay(penaltyPerDay), maxSecDelay(maxSecDelay)
{
}

time_t TransformOrder::getReadyTime() const
{
    return (sentAt + (time_t)maxSecDelay);
}

int TransformOrder::getEstimatedWork() const
{
    return getToDo() * ((int)finalp - (int)initial);
}

int TransformOrder::getPenalty() const
{
    // penalty * (time_finish - max_time_finish)
    return penaltyPerDay * ((long)finishedAt - ((long)sentAt + maxSecDelay)) / DAY_OF_WORK;
}
