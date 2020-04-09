#pragma once
#include <queue>
#include <mutex>
#include <functional>
#include <memory>
#include <queue>
#include <thread>
#include <map>
#include <unordered_map>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include "settings.h"

//?? std::enable_shared_from_this
//class SyncQueue: std::enable_shared_from_this<SyncQueue<Key, Value>> {
template <typename Key, typename Value>
class LockFreeQueue //problem with thread data reordering? can't guarantee consistent data change, cache problem?(
{
public:
    using ExtractorType = std::shared_ptr<std::function<void(Value &value, int &extracted)>>;

    LockFreeQueue(Key id) : id_(id), free_(QUEUE_MAX_CAPACITY), values_(QUEUE_MAX_CAPACITY)
    {
        for (int i = 0; i < QUEUE_MAX_CAPACITY; i++) {
            free_[i] = std::make_pair(std::make_unique<std::mutex>(), true);
        }
#ifdef DEBUG
        std::cout << "Create new LockFreeQueue with id " << id << std::endl;
#endif // DEBUG
    }

    std::function<void(Value &value)> GetInserter()
    {
        auto lambda = [this](Value &value) {
            insert_position_ %= QUEUE_MAX_CAPACITY;
            //std::lock_guard<std::mutex> lock(mtx_);
            std::unique_lock<std::mutex> lock(*free_[insert_position_].first);
            if (free_[insert_position_].second)
            {
                values_[insert_position_] = value, free_[insert_position_].second = false, insert_position_++; //redo ...
                lock.unlock();
                //std::cout << "notify" << std::endl;
                notifier_(id_);
                
            }
        };
        return lambda;
    }

    std::function<void(Value &value, int &extracted)> GetExtractor()
    {
        auto lambda = [this](Value &value, int &extracted) {
            extract_position_ %= QUEUE_MAX_CAPACITY;
            //std::lock_guard<std::mutex> lock(mtx_);
            std::unique_lock<std::mutex> lock(*free_[extract_position_].first);
            if (!free_[extract_position_].second)
            {
                value = values_[extract_position_], free_[extract_position_].second = true, extract_position_++; //redo ....
                extracted = true;
                return;
            }
            extracted = false;
        };
        return lambda;
    }

    typedef std::function<void(Value &value, int &extracted)> function_t;

    void SetNotifier(std::function<void(Key id)> notifier)
    {
        if (notifier_ != nullptr)
        {
            return;
        }
        notifier_ = notifier;
    }

    ~LockFreeQueue()
    {
#ifdef DEBUG
        std::cout << "LockFreeQueue Destructor Call" << std::endl;
#endif // DEBUG
    }

private:
    uint32_t insert_position_{0};
    uint32_t extract_position_{0};
    std::vector <std::pair<std::unique_ptr<std::mutex>, bool>> free_;
    std::vector<Value> values_;
    std::function<void(Key id)> notifier_{nullptr};
    std::mutex mtx_;
    Key id_;
};
