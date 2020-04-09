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
class SyncQueue
{
public:
	using ExtractorType = std::shared_ptr<std::function<void(Value & value, int& extracted)>>;

	SyncQueue(Key id) : id_(id)
	{
#ifdef DEBUG
		std::cout << "Create new SyncQueue with id " << id << std::endl;
#endif // DEBUG
	}

	std::function<void(Value &value)> GetInserter()
	{

		//auto self = std::enable_shared_from_this<SyncQueue<Key, Value>>::shared_from_this();
		auto lambda = [this](Value &value) {
			std::unique_lock<std::mutex> lock(mtx_);
			if (queue_.size() < QUEUE_MAX_CAPACITY)
			{
				queue_.push(value);
				lock.unlock();
				notifier_(id_);
			}
		};
		return lambda;
	}

	std::function<void(Value &value, int &extracted)> GetExtractor()
	{
		auto lambda = [this](Value &value, int &extracted) {
			std::lock_guard<std::mutex> lock(mtx_);
			if (queue_.size())
			{
				value = queue_.front();
				queue_.pop();
				extracted = 1;
				return;
			}
			extracted = 0;
		};
		return lambda;
	}

	typedef std::function<void(Value & value, int& extracted)> function_t;


	std::function<int(void)> GetSizeGetter()
	{
		auto lambda = [this](void) -> int {
			std::lock_guard<std::mutex> lock(mtx_);
			int size = queue_.size();
			return size;
		};

		return lambda;
	}

	void SetNotifier(std::function<void(Key id)> notifier)
	{
		if (notifier_ != nullptr)
		{
			return;
		}
		notifier_ = notifier;
	}

	~SyncQueue()
	{
#ifdef DEBUG
		std::cout << "SyncQueue Destructor Call" << std::endl;
#endif // DEBUG
	}

private:
	std::mutex mtx_;
	std::queue<Value> queue_;
	std::function<void(Key id)> notifier_{nullptr};
	Key id_;
};
