#pragma once
#include <vector>
#include "MultiProducer.h"
#include "IEnqueue.h"
//num_producers - total producers (=queues)
//max_threads - thread bag for producer
//producer in thread ~ max_producers

template <typename Key, typename Value>
class MultiProducersBag
{
public:
	MultiProducersBag(IEnqueue<Key, Value>& processor, size_t num_threads, size_t num_producers, uint32_t delay) :
															processor_(processor), num_producers_(num_producers), num_threads_(num_threads), delay_(delay) {
		
		for (int i = 0; i < num_threads_; i++) {
			//
			int32_t from = i * num_producers_;
			int32_t to = (i + 1) * num_producers_;
			producers_.push_back(std::make_unique < MultiProducer<Key, Value>>(processor, from, to, 0, delay));
			//producers_.push_back(std::make_unique<Producer<Key, Value>>(processor_, i, 0, delay_));
		}
	}

	void Run()
	{
		//roducer.get()->StartProducing();
		for (auto& producer : producers_)
		{
			producer.get()->Run();
		}
	}

	~MultiProducersBag() {
#ifdef DEBUG
		std::cout << "ProducersBag Destructor Call" << std::endl;
#endif // DEBUG


	}
private:
	size_t num_producers_;
	size_t num_threads_;
	std::vector<std::unique_ptr<MultiProducer<Key, Value>>> producers_;
	IEnqueue<Key, Value>& processor_;
	bool running_{ false };
	std::thread th_;
	uint32_t delay_;
};