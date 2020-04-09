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
class LockFreeQueue
{
public:
    using ExtractorType = std::shared_ptr<std::function<void(Value &value, int &extracted)>>;

    LockFreeQueue(Key id) : id_(id), free_(MaxCapacity, true), values_(MaxCapacity)
    {
#ifdef DEBUG
        std::cout << "Create new LockFreeQueue with id " << id << std::endl;
#endif // DEBUG
    }

    std::function<void(Value &value)> GetInserter()
    {
        auto lambda = [this](Value &value) {
            insert_position_ %= MaxCapacity;
            if (free_[insert_position_])
            {
                values_[insert_position_] = value;
                free_[insert_position_] = false;
                notifier_(id_);
                insert_position_++;
            }
        };
        return lambda;
    }

    std::function<void(Value &value, int &extracted)> GetExtractor()
    {
        auto lambda = [this](Value &value, int &extracted) {
            extract_position_ %= MaxCapacity;
            if (!free_[extract_position_])
            {
                value = values_[extract_position_];
                free_[extract_position_] = true;
                extract_position_++;
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
    std::vector<bool> free_;
    std::vector<Value> values_;
    std::function<void(Key id)> notifier_{nullptr};
    Key id_;
};
