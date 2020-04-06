#pragma once
#include <queue>
#include <unordered_set>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <chrono>
#include <map>
#include <cassert>
#include "IPusher.h"
#include "settings.h"

using namespace std::chrono_literals;

//замена notify_queue_.size() на size сильно увеличивает производительность
template <typename Key, typename Value>
class NaiveQueue
{
public:
    NaiveQueue()
    {
       // std::cout << "create Naive Queue" << std::endl;
    }

    void Enqueue(Key id, Value value)
    {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            auto iter = queues_.find(id);
            if (iter != queues_.end())
            {
                if (iter->second.size() < MaxCapacity)
                {
                    iter->second.push(value);
                    notify_queue_.push(id);
                    //notify_set_.insert(id);
                }
            }
            else
            {
                queues_.insert(std::make_pair(id, std::queue<Value>()));
                iter = queues_.find(id);
                if (iter != queues_.end())
                {
                    if (iter->second.size() < MaxCapacity)
                    {
                        iter->second.push(value);
                        notify_queue_.push(id);
                        //notify_set_.insert(id);
                    }
                }
            }
        }
        cv_.notify_one();
    }

    void Poll(IPusher<Key, Value> &manager)
    {

        {
            std::unique_lock<std::mutex> lock(mtx2_);
            cv_.wait_for(lock, 1ms, [this]() { return notify_queue_.size(); });
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            while (notify_queue_.size())
            {
                auto id = notify_queue_.front();
                notify_queue_.pop();
                auto iter = queues_.find(id);
                if (iter != queues_.end())
                {
                    while (iter->second.size())
                    {
                        auto front = iter->second.front();
                        iter->second.pop();
                        manager.TryPush(id, front);
                    }
                }
            }
        }
    }

    /*void Poll(IConsumerManager<Key, Value>& manager)
    {

        std::unique_lock<std::recursive_mutex> lock{ mtx_ };

        if (!notify_set_.size())
        {
            cv_.wait_for(lock, 1ms, [this]() { return notify_set_.size(); });
        }

        for (auto id : notify_set_) {
            //std::cout << id << std::endl;
            auto iter = queues_.find(id);
            if (iter != queues_.end()) {
                while (iter->second.size()) {
                    auto front = iter->second.front();
                    iter->second.pop();
                    manager.TryPush(id, front);
                }
            }
        }
        notify_set_.clear();

    }*/
    /*
    void Poll(IConsumerManager<Key, Value> &manager)
    {

        std::unique_lock<std::recursive_mutex> lock{mtx_};

        if (!notify_queue_.size())
        {
            cv_.wait_for(lock, 10ms, [this]() { return notify_queue_.size(); });
            //waiting_queues_.size();
        }

        if constexpr (Persist)
        {
            PersistRoutine(manager);
        }

        while (notify_queue_.size())
        {

            auto id = notify_queue_.front();
            notify_queue_.pop();
            //inline Dequeue
            auto iter = queues_.find(id);
            if (iter != queues_.end())
            {
                if (iter->second.size())
                {
                    auto front = iter->second.front();
                    if constexpr (!Persist)
                    {
                        iter->second.pop();
                        //lock.unlock();
                        manager.TryPush(id, front);
                        //lock.lock();
                    }
                    else
                    {
                        if (manager.TryPush(id, front))
                        {
                            iter->second.pop();
                        }
                        else
                        {
                            waiting_queues_.insert(id);
                        }
                    }
                }
            }
        }
    }*/

protected:
    void PersistRoutine(IPusher<Key, Value> &manager)
    {
        std::vector<int> vec;
        for (auto id : waiting_queues_)
        {
            auto iter = queues_.find(id);
            if (iter != queues_.end())
            {
                while (iter->second.size())
                {
                    auto front = iter->second.front();

                    if (manager.TryPush(id, front))
                    {
                        iter->second.pop();
                    }
                    else
                    {
                        break;
                    }
                }

                if (iter->second.size() == 0)
                {
                    vec.push_back(id);
                }
            }
        }
        for (auto id : vec)
        {
            waiting_queues_.erase(id);
        }
    }

    bool empty()
    {
        return !size_;
    }

    int32_t size()
    {
        return size_;
    }

    int32_t exceed_capacity()
    {
        return exceed_capacity_;
    }

    std::mutex mtx_;
    std::mutex mtx2_;
    std::condition_variable_any cv_;
    std::unordered_set<Key> notify_set_;
    std::map<Key, std::queue<Value>> queues_;
    std::queue<Key> notify_queue_;
    std::unordered_set<Key> waiting_queues_;
    int32_t exceed_capacity_{0};
    std::unordered_set<Key> updated_keys_;
    int32_t size_{0};
};
