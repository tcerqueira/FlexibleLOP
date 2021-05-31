#include "XmlParser.h"
#include "Orders/Orders.h"
#include "Utils.h"

void appendCommonAttr(pugi::xml_node node, std::shared_ptr<TransformOrder> order);

ScheduleDoc::ScheduleDoc(Scheduler &schedule)
{
    pugi::xml_node schedule_root = this->append_child(SCHEDULE_ROOT_NODE);
    pugi::xml_node order_node, curr_node;
    const std::vector<std::shared_ptr<TransformOrder>> &orders = schedule.getAllOrders();
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    for (int i = 0; i < orders.size(); i++)
    {
        auto t_order = orders[i];

        // Append order node
        order_node = schedule_root.append_child(ORDER_NODE);
        order_node.append_attribute("Number") = orders[i]->getId();

        // Append main node
        curr_node = order_node.append_child(TRANSF_NODE);

        // calculate estimates
        long startedTime = t_order->getTimeStarted();
        long finishedTime = t_order->getTimeFinished();

        if(startedTime == 0) {
            int avgWork = (schedule.getCellWork(1) + schedule.getCellWork(2)) / 2;
            startedTime = t_order->getTimeRcv() + avgWork;
        }
        if(finishedTime == 0) {
            int avgWork = (schedule.getQueueWork(1) + schedule.getQueueWork(2)) / 2;
            finishedTime = startedTime + avgWork > (long)now + avgWork ? startedTime + avgWork : (long)now + avgWork;
        }
        int penalty = calculatePenalty(t_order->getDailyPenalty(), finishedTime, t_order->getTimeSent(), t_order->getMaxDelay(), DAY_OF_WORK);

        // Append specific attributes
        char *from_str = pieceToString(t_order->getInitial());
        char *to_str = pieceToString(t_order->getFinal());
        curr_node.append_attribute("From") = from_str;
        curr_node.append_attribute("To") = to_str;
        curr_node.append_attribute("Penalty") = t_order->getDailyPenalty();
        curr_node.append_attribute("MaxDelay") = t_order->getMaxDelay();
        curr_node.append_attribute("PenaltyIncurred") = penalty;
        free(from_str); free(to_str);

        // common attributes
        curr_node.append_attribute("Quantity") = t_order->getQuantity();
        curr_node.append_attribute("Quantity1") = t_order->getDone();
        curr_node.append_attribute("Quantity2") = t_order->getDoing();
        curr_node.append_attribute("Quantity3") = t_order->getToDo();
        curr_node.append_attribute("Time") = t_order->getTimeSent();
        curr_node.append_attribute("Time1") = t_order->getTimeRcv();
        curr_node.append_attribute("Start") = startedTime;
        curr_node.append_attribute("End") = finishedTime;
    }
}
