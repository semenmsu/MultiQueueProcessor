#pragma once

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
class BruteForceQueue
{
public:
    BruteForceQueue()
    {
    }

    void Enqueue(Key id, Value value)
    {

        std::lock_guard<std::mutex> lock(mtx_);
        auto iter = queues_.find(id);
        if (iter != queues_.end())
        {
            if (iter->second.size() < MaxCapacity)
            {
                iter->second.push(value);
                size_++;
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
                    size_++;
                }
            }
        }

        cv_.notify_one();
    }

    void Poll(IPusher<Key, Value> &manager)
    {

        {
            std::unique_lock<std::mutex> lock(poll_mtx_);
            cv_.wait_for(lock, 1ms, [this]() { return size_; });
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            for (auto &kvp : queues_)
            {
                while (kvp.second.size())
                {
                    auto front = kvp.second.front();
                    kvp.second.pop();
                    manager.TryPush(kvp.first, front);
                }
            }
            size_ = 0;
        }
    }

protected:
    //std::recursive_mutex mtx_;
    std::mutex mtx_;
    std::mutex poll_mtx_;
    std::condition_variable_any cv_;
    std::map<Key, std::queue<Value>> queues_;
    std::queue<Key> notify_queue_;
    std::unordered_set<Key> waiting_queues_;
    int32_t exceed_capacity_{0};
    std::unordered_set<Key> updated_keys_;
    int32_t size_{0};
};
