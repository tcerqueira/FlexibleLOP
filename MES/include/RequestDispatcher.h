#pragma once

#include <functional>
#include <vector>

class Dispatcher
{
public:
    void dispatch(const char* bytes);
    void addOrderListener(std::function<void(const char*)> handler);
    void addStorageListener(std::function<void(const char*)> handler);
    void addScheduleListener(std::function<void(const char*)> handler);

private:
    // vector of handlers
    std::vector<std::function<void(const char*)>> orderHandlers;
    std::vector<std::function<void(const char*)>> storageHandlers;
    std::vector<std::function<void(const char*)>> scheduleHandlers;
};