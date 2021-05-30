#include "Scheduler.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include "Database/Database.h"

#define N_ORDERDIV 8

std::shared_ptr<SubOrder> toSubOrder(const std::shared_ptr<TransformOrder> order);
void separateRoute(std::shared_ptr<SubOrder> sub_order, int iter);
void chooseToolSet(int16_t *tool_set, const std::vector<int16_t> &tools);

Scheduler::Scheduler(Storage *store)
: store(store)
{
    std::make_heap(to_dispatch.begin(), to_dispatch.end(), OrderPriority());
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
    std::lock_guard<std::mutex> lock(transforms_mutex);
    orders_list.push_back(order);
    to_dispatch.push_back(order);
    // TODO: insert order to db
    Database::Get().insertOrder(order);
    std::push_heap(to_dispatch.begin(), to_dispatch.end(), OrderPriority());
}

void Scheduler::addUnloadList(std::vector<std::shared_ptr<UnloadOrder>> &list)
{
    for (std::shared_ptr<UnloadOrder> order : list)
    {
        addUnload(order);
    }
}

void Scheduler::addUnload(std::shared_ptr<UnloadOrder> order)
{
    std::lock_guard<std::mutex> lock(unloads_mutex);
    unload_orders.push_back(order);
    Database::Get().insertUnload(order);
}

std::shared_ptr<UnloadOrder> Scheduler::requestUnload()
{
    std::lock_guard<std::mutex> lock(unloads_mutex);
    int i = 0;
    for(auto unload : unload_orders)
    {
        if(unload->getQuantity() <= store->countPiece(unload->getPiece()))
        {
            dispatched_unloads.push_back(unload);
            unload_orders.erase(unload_orders.begin()+i);
            Database::Get().deleteUnload(unload->getId());
            return unload;
        }
        i++;
    }
    
    return nullptr;
}

std::shared_ptr<SubOrder> Scheduler::popSubOrder(int cell)
{
    std::list<std::shared_ptr<SubOrder>> &sub_orders = (cell == 1) ? cell1_orders : cell2_orders;
    std::shared_ptr<SubOrder> sub_order = nullptr;

    std::list<std::shared_ptr<SubOrder>>::iterator it;
    for(it=sub_orders.begin(); it != sub_orders.end(); ++it)
    {
        sub_order = *it;
        if(sub_order->quantity <= store->countPiece((piece_t)sub_order->init_p))
        {
            sub_orders.erase(it);
            break;
        }
    }
    if(it == sub_orders.end())
        return nullptr;
    
    return sub_order;
}

std::shared_ptr<SubOrder> Scheduler::requestOrderCell(int cell)
{
    std::unique_lock<std::mutex> lock(suborders_mutex);
    static int last_order_numberC[2] = {0,0};
    std::list<std::shared_ptr<SubOrder>> &sub_orders = (cell == 1) ? cell1_orders : cell2_orders;
    std::list<std::shared_ptr<SubOrder>> &disp_orders = (cell == 1) ? cell1_dispatched_orders : cell2_dispatched_orders;
    std::shared_ptr<SubOrder> sub_order = nullptr;

    // optimize cell dead time
    constexpr const int threshold = 500;
    if(cell == 1 && sub_orders.empty() && !cell2_orders.empty() && !cell2_dispatched_orders.empty() && getQueueWork(cell) <= threshold && getTotalWork(2) > threshold) {
        sub_order = popSubOrder(2);
        sub_orders.push_back(sub_order);
    }
    else if(cell == 2 && sub_orders.empty() && !cell1_orders.empty() && !cell1_dispatched_orders.empty() && getQueueWork(cell) <= threshold && getTotalWork(1) > threshold) {
        sub_order = popSubOrder(1);
        sub_orders.push_back(sub_order);
    }
    
    // takes next sub order
    sub_order = popSubOrder(cell);
    if(sub_order != nullptr)
        disp_orders.push_back(sub_order);
    
    // checks if its the first sub order of an order and records that it was sent to the factory
    if(sub_order != nullptr && sub_order->orderID != last_order_numberC[cell-1])
    {
        auto curr_order = getTransform(sub_order->orderID);
        if(curr_order->getDoing() == 0)
            curr_order->sent();
        last_order_numberC[cell-1] = sub_order->orderID;
    }

    lock.unlock();
    return sub_order; 
}

void Scheduler::priority_push_back(int cell, std::shared_ptr<SubOrder> sub_order)
{
    std::lock_guard<std::mutex> lock(suborders_mutex);
    OrderPriority hasPriority;
    std::list<std::shared_ptr<SubOrder>> &sub_orders = (cell == 1) ? cell1_orders : cell2_orders;

    std::list<std::shared_ptr<SubOrder>>::iterator it;
    for(it=sub_orders.begin(); it != sub_orders.end(); ++it)
    {
        if(hasPriority(sub_order, *it))   // sub_order has more priority over *it
        {
            sub_orders.insert(it, sub_order);
            break;
        }
    }
    if(it == sub_orders.end()) {
        sub_orders.push_back(sub_order);
    }
}

void Scheduler::schedule()
{
    std::lock_guard<std::mutex> lock(transforms_mutex);
    int work_cell1, work_cell2;
    while(to_dispatch.size() > 0)
    {
        std::pop_heap(to_dispatch.begin(), to_dispatch.end());
        auto order = to_dispatch.back();
        to_dispatch.pop_back();

        work_cell1 = getTotalWork(1);
        work_cell2 = getTotalWork(2);

        auto sub_order = toSubOrder(order);
        if(sub_order->tools.size() <= 2 && sub_order->tools.size() > 0)
        {
            for(int i = 0; i<sub_order->to_do; i++)
            {
                auto order_aux = std::make_shared<SubOrder>(*sub_order);
                order_aux->quantity = 1;
                order_aux->to_do = 1;
                order_aux->work = (sub_order->work * order_aux->quantity) / sub_order->quantity;
                separateRoute(order_aux, i);
                if (work_cell1 <= work_cell2)
                {
                    // cell1_orders.push_back(order_aux);
                    priority_push_back(1, order_aux);
                    // std::push_heap(cell1_orders.begin(), cell1_orders.end(), OrderPriority());
                }
                else
                {
                    // cell2_orders.push_back(order_aux);
                    priority_push_back(2, order_aux);
                    // std::push_heap(cell2_orders.begin(), cell2_orders.end(), OrderPriority());
                }
            }
        }
        else if(sub_order->warehouse_intermediate)
        {
            sub_order->warehouse_intermediate = false;
            auto order_itm = std::make_shared<SubOrder>(*sub_order);
            sub_order->final_p = sub_order->piece_intermediate;
            if (work_cell1 <= work_cell2)
            {
                // cell1_orders.push_back(sub_order);
                priority_push_back(1, sub_order);
                // std::push_heap(cell1_orders.begin(), cell1_orders.end(), OrderPriority());
            }
            else
            {
                // cell2_orders.push_back(sub_order);
                priority_push_back(2, sub_order);
                // std::push_heap(cell2_orders.begin(), cell2_orders.end(), OrderPriority());
            }
            
            order_itm->init_p = sub_order->piece_intermediate;
            order_itm->tools.clear();
            memset(order_itm->path, 0, sizeof(sub_order->path[0])*8);
            memset(order_itm->tool_time, 0, sizeof(sub_order->tool_time[0])*8);
            memset(order_itm->piece_seq, 0, sizeof(sub_order->piece_seq[0])*8);
            if(sub_order->tools.size() >= 5){
                order_itm->tools.push_back(sub_order->tools[4]);
                order_itm->tool_time[0] = sub_order->tool_time[5];
                order_itm->piece_seq[0] = sub_order->piece_seq[5];
                order_itm->path[1] = 0;
                if(sub_order->tools.size() == 6){
                    order_itm->tools.push_back(sub_order->tools[6]);
                    order_itm->tool_time[1] = sub_order->tool_time[5];
                    order_itm->piece_seq[0] = sub_order->piece_seq[5];
                    order_itm->path[2] = 0;
                }

            }
            chooseToolSet(order_itm->tool_set, order_itm->tools);
            for(int i = 0; i<order_itm->to_do; i++)
            {
                auto order_aux = std::make_shared<SubOrder>(*order_itm);
                order_aux->quantity = 1;
                order_aux->to_do = 1;
                order_aux->work = (order_itm->work * order_aux->quantity) / order_itm->quantity;
                separateRoute(order_aux, i);
                if (work_cell1 <= work_cell2)
                {
                    // cell1_orders.push_back(order_aux);
                    priority_push_back(1, order_aux);
                    // std::push_heap(cell1_orders.begin(), cell1_orders.end(), OrderPriority());
                }
                else
                {
                    // cell2_orders.push_back(order_aux);
                    priority_push_back(2, order_aux);
                    // std::push_heap(cell2_orders.begin(), cell2_orders.end(), OrderPriority());
                }
            }
        }
        else if(sub_order->quantity >= N_ORDERDIV && std::abs(work_cell1-work_cell2) < 150)
        {
            auto order_c1 = std::make_shared<SubOrder>(*sub_order);
            auto order_c2 = std::make_shared<SubOrder>(*sub_order);
            order_c1->quantity = sub_order->quantity / 2 + sub_order->quantity % 2;
            order_c1->to_do = sub_order->quantity / 2 + sub_order->quantity % 2;
            order_c1->work = (sub_order->work * order_c1->quantity) / sub_order->quantity;
            // cell1_orders.push_back(order_c1);
            priority_push_back(1, order_c1);
            order_c2->quantity = sub_order->quantity / 2;
            order_c2->to_do = sub_order->quantity / 2 + sub_order->quantity % 2;
            order_c2->work = (sub_order->work * order_c2->quantity) / sub_order->quantity;
            // cell2_orders.push_back(order_c2);
            priority_push_back(2, order_c2);
        }
        else
        {
            if (work_cell1 <= work_cell2)
            {
                // cell1_orders.push_back(sub_order);
                priority_push_back(1, sub_order);
                // std::push_heap(cell1_orders.begin(), cell1_orders.end(), OrderPriority());
            }
            else
            {
                // cell2_orders.push_back(sub_order);
                priority_push_back(2, sub_order);
                // std::push_heap(cell2_orders.begin(), cell2_orders.end(), OrderPriority());
            }
        }

    }
    // MES_TRACE("Work cell 1:{}  Work cell 2:{}", work_cell1, work_cell2);
    to_dispatch.clear();
}

void Scheduler::clean()
{
    std::lock_guard<std::mutex> lock1(transforms_mutex);
    std::lock_guard<std::mutex> lock2(unloads_mutex);
    if(!cell1_orders.empty() || !cell2_orders.empty() || !cell1_dispatched_orders.empty() || !cell2_dispatched_orders.empty())
    {
        MES_WARN("Scheduler still has pending work, wait for it to finish.");
        return;
    }
    orders_list.clear();
    to_dispatch.clear();
    unload_orders.clear();
    dispatched_unloads.clear();
}

int Scheduler::getCellWork(int cell) const
{
    // std::lock_guard<std::mutex> lock(suborders_mutex);
    int work = 0;
    if (cell == 1)
    {
        for (auto order : cell1_orders)
            work += WORK_TRANSFORM * (order->work);
        work += WORK_CHANGETOOLS * cell1_orders.size();
    }
    else
    {
        for (auto order : cell2_orders)
            work += WORK_TRANSFORM * (order->work);
        for(auto unload : unload_orders)
            work += WORK_UNLOAD * unload->getQuantity();
        work += WORK_CHANGETOOLS * cell2_orders.size();
    }

    return work;
}

int Scheduler::getQueueWork(int cell) const
{
    // std::lock_guard<std::mutex> lock(suborders_mutex);
    int work = 0;
    const std::list<std::shared_ptr<SubOrder>> &disp_orders = (cell == 1) ? cell1_dispatched_orders : cell2_dispatched_orders;

    for (auto order : disp_orders) {
        work += WORK_TRANSFORM * (order->work);
    }
    work += WORK_CHANGETOOLS * disp_orders.size();

    return work;
}

int Scheduler::getTotalWork(int cell) const
{
    return getCellWork(cell) + getQueueWork(cell);
}

bool Scheduler::OrderPriority::operator()(const std::shared_ptr<TransformOrder> o1, const std::shared_ptr<TransformOrder> o2) const
{
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    long p1 = (o1->getReadyTime() - now - o1->getEstimatedWork() * WORK_TRANSFORM) * o1->getDailyPenalty(); /// DAY_OF_WORK;
    long p2 = (o2->getReadyTime() - now - o2->getEstimatedWork() * WORK_TRANSFORM) * o2->getDailyPenalty(); /// DAY_OF_WORK;

    return p1 < p2;
}

bool Scheduler::OrderPriority::operator()(const std::shared_ptr<SubOrder> o1, const std::shared_ptr<SubOrder> o2) const
{
    // MES_TRACE("p{}:{} p{}:{}", o1->orderID, o1->priority, o2->orderID, o2->priority);
    return o1->priority < o2->priority;
}

void Scheduler::updatePieceStarted(int cell, int number)
{
    // auto order = this->getTransform(number);
    // if(order == nullptr)
    //     return;

    // if(order->getDoing() == 0) 
    //     order->started();

    // order->pieceDoing();
    // TODO async query
    // Database::Get().updateStorage((int) order->getInitial(), store->countPiece(order->getInitial()));
}

void Scheduler::updatePieceFinished(int cell, int number)
{
    // auto order = this->getTransform(number);
    // if(order == nullptr)
    //     return;

    // order->pieceDone();

    // order->pieceDone();
    // if(order->getDone() == order->getQuantity())
    // {
    //     Database::Get().deleteOrder(order->getId());
    //     MES_TRACE("Deleted order {}", order->getId());
    // }

    // if(order->getDone() == order->getQuantity()) 
    //     order->finished();

    // update dispatched lists
    std::lock_guard<std::mutex> lock(suborders_mutex);
    std::shared_ptr<SubOrder> sub_order = nullptr;
    std::list<std::shared_ptr<SubOrder>> &disp_orders = (cell == 1) ? cell1_dispatched_orders : cell2_dispatched_orders;
    
    std::list<std::shared_ptr<SubOrder>>::iterator it;
    for(it=disp_orders.begin(); it != disp_orders.end(); ++it)
    {
        sub_order = *it;
        if(sub_order->orderID == number){
            if(--sub_order->to_do == 0)
                disp_orders.erase(it);
            sub_order->work = sub_order->calculateWork();
            break;
        }
    }
    if(it == cell1_dispatched_orders.end())
        MES_WARN("SubOrder number:{} in cell:{} not found in dispatched orders.", number, cell);
    
    // TODO async query
    // Database::Get().updateStorage((int) order->getFinal(), store->countPiece(order->getFinal()));
}

std::shared_ptr<TransformOrder> Scheduler::getTransform(int number) 
{
    std::lock_guard<std::mutex> lock(transforms_mutex);
    for(auto order : orders_list)
        if(order->getId() == number) return order;
    
    return nullptr;
}

// ########################### AUXILIAR FUNCTIONS ####################################
// ###################################################################################

void chooseTools(std::shared_ptr<SubOrder> sub_order, std::vector<int16_t>& tools, const std::shared_ptr<TransformOrder> next_order);
void chooseRoute(std::shared_ptr<SubOrder> sub_order, const std::vector<int16_t> &tools);

std::shared_ptr<SubOrder> toSubOrder(const std::shared_ptr<TransformOrder> order)
{
    auto sub_order = std::make_shared<SubOrder>();
    sub_order->orderID = (int16_t)order->getId();
    sub_order->init_p = (uint16_t)order->getInitial();
    sub_order->final_p = (uint16_t)order->getFinal();
    sub_order->quantity = (int16_t)order->getQuantity();
    sub_order->to_do = (int16_t)order->getToDo();
    sub_order->done = (int16_t)order->getDone();
    std::vector<int16_t> tools; tools.reserve(6);
    chooseTools(sub_order, sub_order->tools, order);
    chooseToolSet(sub_order->tool_set, sub_order->tools);
    chooseRoute(sub_order, sub_order->tools);
    sub_order->warehouse_intermediate = false;
    for(int i = 0; i<8; i++){
        if(sub_order->path[i]==5){
            sub_order->warehouse_intermediate = true;
        }
    }
    sub_order->penalty = order->getDailyPenalty();
    sub_order->priority = sub_order->calculatePriority();
    sub_order->readyTime = order->getReadyTime();
    sub_order->work = order->getEstimatedWork();

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
            sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            break;
        case P2:
            tools.push_back(1);
            sub_order->tool_time[i+intermediate] = 15000;
            piece_act = P3;
            sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            break;
        case P3:
            tools.push_back(2);
            sub_order->tool_time[i+intermediate] = 15000;
            piece_act = P4;
            sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            break;
        case P4:
            tools.push_back(0);
            sub_order->tool_time[i+intermediate] = 15000;
            piece_act = P5;
            sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            break;
        case P5:
            if(i==4){
                sub_order->piece_intermediate = 5;
                sub_order->tool_time[i+intermediate] = 0;
                sub_order->piece_seq[i+intermediate] = 0;
                intermediate++;
            }
            if(next_order->getFinal() == P9){
                tools.push_back(2);
                sub_order->tool_time[i+intermediate] = 30000;
                piece_act = P9;
                sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            }
            else{
                tools.push_back(1);
                sub_order->tool_time[i+intermediate] = 30000;
                piece_act = P6;
                sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            }
            break;
        case P6:
            if(i==4){
                sub_order->piece_intermediate = 6;
                sub_order->tool_time[i+intermediate] = 0;
                intermediate++;
                sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            }
            if(next_order->getFinal() == P8){
                tools.push_back(0);
                sub_order->tool_time[i+intermediate] = 15000;
                piece_act = P8;
                sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            }
            else{
                tools.push_back(2);
                sub_order->tool_time[i+intermediate] = 30000;
                piece_act = P7;
                sub_order->piece_seq[i+intermediate] = (int16_t) piece_act;
            }
            break;
        }
        i++;
    }
    sub_order->piece_seq[i+intermediate+1] = (int16_t) sub_order->final_p;
    
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
    if(tools.size() <= 2){
        for(int i = tools.size(); i < 4; i++)
        tool_set[i] = tool_set[i-tools.size()]; 
    }
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

void separateRoute(std::shared_ptr<SubOrder> sub_order, int iter)
{
    if(sub_order->tools.size() == 1)
    {
        sub_order->path[0] =(iter % 4) + 1;
    }
    else{
        sub_order->path[0] = (iter % 2) * 2 + 1;
        sub_order->path[1] = (iter % 2) * 2 + 2;
    }

    // int maq_div = 4/sub_order->tools.size();
    // for(int i = 0; i<sub_order->tools.size(); i++)
    // {
    //     sub_order->path[i] = maq_div - (iter % maq_div) * sub_order->tools.size() + 1 + i;
    // }
}