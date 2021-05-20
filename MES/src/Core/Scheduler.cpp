#include "Scheduler.h"
#include <algorithm>

std::shared_ptr<SubOrder> toSubOrder(const std::shared_ptr<TransformOrder> order);

Scheduler::Scheduler(Storage *store)
: store(store)
{
    std::make_heap(to_dispatch.begin(), to_dispatch.end(), OrderPriority());
    // std::make_heap(t1_orders.begin(), t1_orders.end(), OrderPriority());
    // std::make_heap(t2_orders.begin(), t2_orders.end(), OrderPriority());
}

void Scheduler::addOrderList(std::vector<std::shared_ptr<TransformOrder>> &list)
{
    for (std::shared_ptr<TransformOrder> order : list)
    {
        addTransform(order);
    }
}

void Scheduler::addTransform(std::shared_ptr<TransformOrder> order)
{
    // TODO: priority insertion
    const std::lock_guard<std::mutex> lock(transformVec_mutex);
    orders_list.push_back(order);
    to_dispatch.push_back(order);
    std::push_heap(to_dispatch.begin(), to_dispatch.end(), OrderPriority());
}

void Scheduler::addUnload(std::shared_ptr<UnloadOrder> order)
{
    const std::lock_guard<std::mutex> lock(unloadVec_mutex);
    u_orders.push_back(order);
}

std::shared_ptr<UnloadOrder> Scheduler::popUnload()
{
    int i = 0;
    for(auto unload : u_orders)
    {
        if(unload->getQuantity() <= store->countPiece(unload->getPiece()))
        {
            dispatched_unloads.push_back(unload);
            u_orders.erase(u_orders.begin()+i);
            return unload;
        }
        i++;
    }
    
    return nullptr;
}

std::shared_ptr<SubOrder> Scheduler::popOrderCell(int cell)
{
    if(cell == 1)
    {
        int i = 0;
        for(std::list<std::shared_ptr<SubOrder>>::iterator it=t1_orders.begin(); it != t1_orders.end(); ++it)
        {
            std::shared_ptr<SubOrder> sub_order = *it;
            if(sub_order->quantity <= store->countPiece((piece_t)sub_order->init_p))
            {
                t1_orders.erase(it);
                return sub_order;
            }
            i++;
        }
    }
    else{
        int i = 0;
        for(std::list<std::shared_ptr<SubOrder>>::iterator it=t2_orders.begin(); it != t2_orders.end(); ++it)
        {
            std::shared_ptr<SubOrder> sub_order = *it;
            if(sub_order->quantity <= store->countPiece((piece_t)sub_order->init_p))
            {
                t2_orders.erase(it);
                return sub_order;
            }
            i++;
        }
    }
    return nullptr;
}

void Scheduler::schedule()
{
    while(to_dispatch.size() > 0)
    {
        std::pop_heap(to_dispatch.begin(), to_dispatch.end());
        auto order = to_dispatch.back();
        to_dispatch.pop_back();

        auto sub_order = toSubOrder(order);

        if (getTotalWork(1) <= getTotalWork(2))
        {
            t1_orders.push_back(sub_order);
            // std::push_heap(t1_orders.begin(), t1_orders.end(), OrderPriority());
        }
        else
        {
            t2_orders.push_back(sub_order);
            // std::push_heap(t2_orders.begin(), t2_orders.end(), OrderPriority());
        }
    }
    MES_TRACE("Work cell 1:{}  Work cell 2:{}", getTotalWork(1), getTotalWork(2));
    to_dispatch.clear();
}

int Scheduler::getTotalWork(int cell)
{
    int work = 0;
    if (cell == 1)
    {
        for (auto order : t1_orders)
            work += WORK_TRANSFORM * (order->work);
        work += WORK_CHANGETOOLS * t1_orders.size();
    }
    else
    {
        for (auto order : t2_orders)
            work += WORK_TRANSFORM * (order->work);
        for(auto unload : u_orders)
            work += WORK_UNLOAD * unload->getQuantity();
        work += WORK_CHANGETOOLS * t2_orders.size();
    }

    return work;
}

bool Scheduler::OrderPriority::operator()(const std::shared_ptr<TransformOrder> o1, const std::shared_ptr<TransformOrder> o2) const
{
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    long p1 = (o1->getReadyTime() - now - o1->getEstimatedWork() * WORK_TRANSFORM) * o1->getDailyPenalty();
    long p2 = (o2->getReadyTime() - now - o2->getEstimatedWork() * WORK_TRANSFORM) * o2->getDailyPenalty();

    return p1 > p2;
}

bool Scheduler::OrderPriority::operator()(const std::shared_ptr<SubOrder> o1, const std::shared_ptr<SubOrder> o2) const
{
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    long p1 = (o1->readyTime - now - o1->work * WORK_TRANSFORM) * o1->penalty;
    long p2 = (o2->readyTime - now - o2->work * WORK_TRANSFORM) * o2->penalty;

    return p1 > p2;
}

void Scheduler::updatePieceStarted(int cell, int number)
{
    // store->subCount(piece, 1);
    auto order = this->getTransform(number);
    if(order == nullptr)
        return;

    if(order->getDoing() == 0) 
        order->started();

    order->pieceDoing();
}

void Scheduler::updatePieceFinished(int cell, int number)
{
    auto order = this->getTransform(number);
    if(order == nullptr)
        return;

    if(order->getDone() == order->getQuantity()) 
        order->finished();

    order->pieceDone();
}

bool Scheduler::hasTransform(int cell) const
{
    return cell == 1 ? !t1_orders.empty() : !t2_orders.empty();
}

bool Scheduler::hasUnload() const
{
    return !u_orders.empty();
}

std::shared_ptr<TransformOrder> Scheduler::getTransform(int number) 
{
    for(auto order : orders_list)
        if(order->getId() == number) return order;
    
    return nullptr;
}

// TransformOrder Scheduler::popOrder()
// {
//     TransformOrder order = --(*t1_orders.front());
//     // if all orders dispatched
//     if(order.getDoing() + order.getDone() >= order.getQuantity()){
//         MES_TRACE("Order {} dispatched.", order);
//         // TODO: add it to dispatched list
//     }

//     return order;
// }
// ########################### AUXILIAR FUNCTIONS ####################################
// ###################################################################################
void chooseTools(std::shared_ptr<SubOrder> sub_order, std::vector<int16_t>& tools, const std::shared_ptr<TransformOrder> next_order);
void chooseToolSet(int16_t *tool_set, const std::vector<int16_t> &tools);
void chooseRoute(std::shared_ptr<SubOrder> sub_order, const std::vector<int16_t> &tools);

std::shared_ptr<SubOrder> toSubOrder(const std::shared_ptr<TransformOrder> order)
{
    auto sub_order = std::make_shared<SubOrder>();
    sub_order->orderID = (int16_t)order->getId();
    sub_order->init_p = (uint16_t)order->getInitial();
    sub_order->quantity = (int16_t)order->getQuantity();
    sub_order->to_do = (int16_t)order->getToDo();
    sub_order->done = (int16_t)order->getDone();
    // sub_ortder->tool_set = ;
    std::vector<int16_t> tools; tools.reserve(6);
    chooseTools(sub_order, tools, order);
    chooseToolSet(sub_order->tool_set, tools);
    // sub_order->path = ;
    chooseRoute(sub_order, tools);
    // sub_order->tool_time = ;
    // sub_order->warehouse_intermediate = ;
    sub_order->warehouse_intermediate = false;
    for(int i = 0; i<8; i++){
        if(sub_order->path[i]==5){
            sub_order->warehouse_intermediate = true;
        }
    }
    // sub_order->piece_intermediate = ;
    sub_order->readyTime = order->getReadyTime();
    sub_order->work = order->getEstimatedWork();
    sub_order->penalty = order->getDailyPenalty();

    return sub_order;
}

// get tools
void chooseTools(std::shared_ptr<SubOrder> sub_order, std::vector<int16_t>& tools, const std::shared_ptr<TransformOrder> next_order)
{
    piece_t piece_act = next_order->getInitial();
    int i = 0, intermediate = 0;
    while(piece_act != next_order->getFinal()){   
        switch (piece_act){     //Find next tool (incomplete)
        case P1:
            tools.push_back(0);
            sub_order->tool_time[i+intermediate] = 15000;
            piece_act = P2;
            break;
        case P2:
            tools.push_back(1);
            sub_order->tool_time[i+intermediate] = 15000;
            piece_act = P3;
            break;
        case P3:
            tools.push_back(2);
            sub_order->tool_time[i+intermediate] = 15000;
            piece_act = P4;
            break;
        case P4:
            tools.push_back(0);
            sub_order->tool_time[i+intermediate] = 15000;
            piece_act = P5;
            break;
        case P5:
            if(i==4){
                sub_order->piece_intermediate = 5;
                sub_order->tool_time[i+intermediate] = 0;
                intermediate++;
            }
            if(next_order->getFinal() == P9){
                tools.push_back(2);
                sub_order->tool_time[i+intermediate] = 30000;
                piece_act = P9;
            }
            else{
                tools.push_back(1);
                sub_order->tool_time[i+intermediate] = 30000;
                piece_act = P6;
            }
            break;
        case P6:
            if(i==4){
                sub_order->piece_intermediate = 6;
                sub_order->tool_time[i+intermediate] = 0;
                intermediate++;
            }
            if(next_order->getFinal() == P8){
                tools.push_back(0);
                sub_order->tool_time[i+intermediate] = 15000;
                piece_act = P8;
            }
            else{
                tools.push_back(2);
                sub_order->tool_time[i+intermediate] = 30000;
                piece_act = P7;
            }
            break;
        }
        i++;
    }
    
}

// choose toolset
void chooseToolSet(int16_t *tool_set, const std::vector<int16_t> &tools)
{
    for(int i = 0; i<tools.size(); i++){
        if(i <= 3){
            tool_set[i] = tools[i];
            continue;
        }
        for(int j = 0; j < 4; j++){
            if(j == 4-1){
                tool_set[j] = tools[i]; //in case there is a missing tool
            }
            if(tools[i]==tool_set[j]){
                break;
            }
        }
    }
    // if(tools.size() <= 2){
    //     for(int i = tools.size(); i < 4; i++)
    //     tool_set[i] = tool_set[i-tools.size()]; 
    // }
}

// choose route
void chooseRoute(std::shared_ptr<SubOrder> sub_order, const std::vector<int16_t> &tools)
{
    int intermediate = 0;
    int mac_act = 0; //starts at warehouse

    for(int i = 0; i<tools.size(); i++){
        for(int j = mac_act; j < 4; j++){   //piece can't go back to other conveyors
            if(tools[i] == sub_order->tool_set[j]){
                sub_order->path[i+intermediate] = (j+1);
                mac_act = j;
                break;
            }
            else{
                if(j == 4-1){
                    sub_order->path[i] = 5;
                    intermediate++;
                    // order.warehouse_intermediate = true;
                    mac_act = 0;
                    j = 0;
                }
            }
        }
    }
}