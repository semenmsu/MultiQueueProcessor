#pragma once
#include <thread>
#include <functional>
#include "EventQueue.h"
#include "Dispatcher.h"
#include "IEnqueue.h"
#include "ISubscriber.h"
#include "ThreadPrioritizer.h"
#include "settings.h"


template <typename Key, typename Value,
		  class Queue = EventQueue<Key, Value>,
		  class Dispatcher = Dispatcher<Key, Value>>
class MultiQueueProcessor : public IEnqueue<Key, Value>, public ISubscriber<Key, Value>
{
public:
	MultiQueueProcessor() : running_{true}, th_(std::bind(&MultiQueueProcessor::Process, this)) {}

	void Subscribe(Key id, std::shared_ptr<IConsumer<Key, Value>> consumer) override
	{
		dispatcher_.Subscribe(id, consumer);
	}

	void Unsubscribe(Key id) override
	{
		dispatcher_.Unsubscribe(id);
	}

	void Enqueue(Key id, Value &value) override
	{
		queue_.Enqueue(id, value);
	}

	void StopProcessing()
	{
		running_ = false;
	}

	~MultiQueueProcessor()
	{
		StopProcessing();
		if (th_.joinable())
		{
			th_.join();
		}
#ifdef DEBUG
		std::cout << "MultiQueueProcessor Destructor Call" << std::endl;
#endif // DEBUG
	}

protected:
	void Process()
	{
		ThreadPrioritizer priorityizer(HIGH_PRIORITY);

		while (running_)
		{
			queue_.Poll(dispatcher_);
		}
		
	}

private:
	bool running_;
	std::thread th_;
	Queue queue_;
	Dispatcher dispatcher_;
	std::function<void(int)> error_handler_;
};
