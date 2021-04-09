#include "XmlParser.h"
#include "Orders.h"
#include "Utils.h"

void appendCommonAttr(pugi::xml_node node, Order *order);

ScheduleDoc::ScheduleDoc(const Scheduler &schedule)
{
    pugi::xml_node schedule_root = this->append_child(SCHEDULE_ROOT_NODE);
    pugi::xml_node order_node, curr_node;

    int len = schedule.orderSize();
    for (int i = 0; i < len; i++)
    {
        auto t_order = dynamic_cast<TransformOrder *>(schedule.orderAt(i));
        auto u_order = dynamic_cast<UnloadOrder *>(schedule.orderAt(i));
        if (t_order == nullptr && u_order == nullptr)
        {
            MES_TRACE("dafuq");
            return;
        }
        // Append order node
        order_node = schedule_root.append_child(ORDER_NODE);
        order_node.append_attribute("Number") = schedule.orderAt(i)->getId();

        // case its a Transform Order
        if (t_order != nullptr)
        {
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
        }
        // case its a Unload Order
        else if (u_order != nullptr)
        {
            // Append main node
            curr_node = order_node.append_child(UNLOAD_NODE);
            // Append specific attributes
            char* type_str = pieceToString(u_order->getPiece());
            char* dest_str = destToString(u_order->getDest());
            curr_node.append_attribute("Type") = type_str;
            curr_node.append_attribute("Destination") = dest_str;
            free(type_str); free(dest_str);
        }
        // Append common attributes
        appendCommonAttr(curr_node, schedule.orderAt(i));
    }
}

void appendCommonAttr(pugi::xml_node node, Order *order)
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
