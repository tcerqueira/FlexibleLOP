#include "XmlParser.h"
#include "Orders/Orders.h"
#include "Utils.h"

void appendCommonAttr(pugi::xml_node node, std::shared_ptr<TransformOrder> order);

ScheduleDoc::ScheduleDoc(Scheduler &schedule)
{
    pugi::xml_node schedule_root = this->append_child(SCHEDULE_ROOT_NODE);
    pugi::xml_node order_node, curr_node;
    const std::vector<std::shared_ptr<TransformOrder>> &ordersC1 = schedule.getAllOrders();

    for (int i = 0; i < ordersC1.size(); i++)
    {
        auto t_order = ordersC1[i];

        // Append order node
        order_node = schedule_root.append_child(ORDER_NODE);
        order_node.append_attribute("Number") = ordersC1[i]->getId();

        // Append main node
        curr_node = order_node.append_child(TRANSF_NODE);

        // Append specific attributes
        char *from_str = pieceToString(t_order->getInitial());
        char *to_str = pieceToString(t_order->getFinal());
        curr_node.append_attribute("From") = from_str;
        curr_node.append_attribute("To") = to_str;
        curr_node.append_attribute("Penalty") = t_order->getDailyPenalty();
        curr_node.append_attribute("MaxDelay") = t_order->getMaxDelay();
        curr_node.append_attribute("PenaltyIncurred") = t_order->getPenalty();
        free(from_str); free(to_str);

        // Append common attributes
        appendCommonAttr(curr_node, ordersC1[i]);
    }
}

void appendCommonAttr(pugi::xml_node node, std::shared_ptr<TransformOrder> order)
{
    node.append_attribute("Quantity") = order->getQuantity();
    node.append_attribute("Quantity1") = order->getDone();
    node.append_attribute("Quantity2") = order->getDoing();
    node.append_attribute("Quantity3") = order->getToDo();
    node.append_attribute("Time") = order->getTimeSent();
    node.append_attribute("Time1") = order->getTimeRcv();
    node.append_attribute("Start") = order->getTimeStarted();
    node.append_attribute("End") = order->getTimeFinished();
}
