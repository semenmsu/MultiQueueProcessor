#pragma once
#include <map>
#include <thread>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <chrono>
#include "IPusher.h"
#include "ISubscriber.h"

using namespace std::chrono_literals;

template <typename Key, typename Value>
//class ConsumerManager : public IConsumerManager<Key, Value>
class Dispatcher: public ISubscriber<Key, Value> , public IPusher<Key,Value>
{
public:
    void Subscribe(Key id, std::shared_ptr<IConsumer<Key, Value>> consumer) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!consumers.count(id))
        {
            consumers.emplace(id, consumer);
        }
    }

    void Unsubscribe(Key id) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (consumers.count(id))
        {
            consumers.erase(id);
        }
    }

    bool TryPush(Key id, Value& value) override
    {
        
        std::unique_lock<std::mutex> lock(mtx);
        
        auto iter = consumers.find(id);
        
        if (iter == consumers.end())
        {
            return false;
        }
        
        
        auto consumer_ptr = iter->second.lock();
        

        if (consumer_ptr)
        {
            lock.unlock();
            consumer_ptr->Consume(id, value);

            return true;
        }
        else {
            consumers.erase(id);
        }
        return false;
    }

    ~Dispatcher() {
#ifdef DEBUG
        std::cout << "Dispatcher Destructor Call" << std::endl;
#endif // DEBUG

    }

protected:
    std::unordered_map<Key, std::weak_ptr<IConsumer<Key, Value>>> consumers;
    std::mutex mtx;
};