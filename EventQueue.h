#pragma once
#include <queue>
#include <mutex>
#include <functional>
#include <memory>
#include <queue>
#include <thread>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>
#include <chrono>
#include "IPusher.h"
#include "SyncQueue.h"
#include "settings.h"

using namespace std::chrono_literals;

template <typename Key>
class NotifyQueue
{
public:
	void Enqueue(Key &value)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		//queue_.push(value);
		set_.insert(value);
		//possible make some optmizations, when queueu_ to big
	}

	void CopyPartly(std::vector<Key> &to, size_t max_elements)
	{
		//std::cout << "CopyParly " << to.size() << std::endl;
		std::lock_guard<std::mutex> lock(mtx_);
		uint32_t elements = 0;
		while (set_.size())
		{
			auto iter = set_.begin();
			to.push_back(*iter);
			set_.erase(iter);
			if (elements++ > max_elements)
			{
				return;
			}
		}
	}

	void Swap(std::unordered_set<Key> &to)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		set_.swap(to);
	}

	void Dequeue(Key &value, int &extracted)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		if (set_.size() == 0)
		{
			extracted = 0;
			return;
		}
		auto iter = set_.begin();
		value = *iter;
		set_.erase(iter);
		extracted = 1;
	};

	size_t size()
	{
		std::lock_guard<std::mutex> lock(mtx_);
		auto set_size = set_.size();
		return set_size;
	}

private:
	std::mutex mtx_;
	//std::queue<Key> queue_;
	std::unordered_set<Key> set_;
};

template <typename Key, typename Value, class BaseQueue = SyncQueue<Key, Value>>
class EventQueue
{
public:
	struct CacheValue
	{
		Value value;
		int32_t dirty;
	};

	EventQueue()
	{
		notifier_ = [this](Key id) {
			this->Notify(id);
		};
	}
	void Enqueue(Key id, Value value)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		auto iter = inserts_.find(id);
		if (iter != inserts_.end())
		{
			lock.unlock();
			iter->second(value);
		}
		else
		{
			//auto ptr = new SyncQueue<Key, Value>(id);
			auto ptr = new BaseQueue(id);
			ptr->SetNotifier(notifier_);
			globalQueueTable_.emplace(id, ptr);
			auto insert = ptr->GetInserter();
			inserts_.emplace(id, insert);
			auto extractor = ptr->GetExtractor();
			extractors_.emplace(id, extractor);
			lock.unlock();
			insert(value);
		}
	}

	void Notify(Key id)
	{
		notifyQueue_.Enqueue(id);
		cv_.notify_one();
	}

	//одному потоку может не доставаться ничего, надо ему дать приоритет
#define MAX_PREFETCH_KEYS 200
	void Poll(IPusher<Key, Value> &manager)
	{
		std::unordered_set<Key> ids;
		notifyQueue_.Swap(ids);
		{
			std::unique_lock<std::mutex> lock(mtx_);
			for (auto& id : ids)
			{

				auto iter = extractors_.find(id);
				if (iter != extractors_.end())
				{
					while (true)
					{
						int32_t extracted = 0;
						Value extracted_value{};
						iter->second(extracted_value, extracted);

						if (!extracted)
						{
							break;
						}
						manager.TryPush(id, extracted_value);
					}
				}
			}
		}
		
		{
			std::unique_lock<std::mutex> lock(mtx2_);
			cv_.wait_for(lock, 1ms, [this]() {
				return !!notifyQueue_.size();
			});
		}
	}

	~EventQueue()
	{
		for (auto &kvp : globalQueueTable_)
		{
			delete kvp.second;
		}
#ifdef DEBUG
		std::cout << "EventQueue Destructor Call " << std::endl;
#endif // DEBUG
	}

private:
	std::unordered_map<Key, BaseQueue *> globalQueueTable_;
	std::unordered_map<Key, std::function<void(Value &value)>> inserts_;
	std::unordered_map<Key, std::function<void(Value &value, int &extracted)>> extractors_;
	//std::unordered_map<Key, std::function<int(void)>> size_getters_;
	std::unordered_map<Key, CacheValue> cache_;
	std::function<void(Key)> notifier_;
	NotifyQueue<Key> notifyQueue_;
	std::mutex mtx_;
	std::mutex mtx2_;
	std::condition_variable_any cv_;
	uint32_t size_;
};
