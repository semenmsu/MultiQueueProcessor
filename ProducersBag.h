#pragma once
#include <vector>
#include "Producer.h"
#include "IEnqueue.h"

template <typename Key, typename Value>
class ProducersBag
{
public:
	ProducersBag(IEnqueue<Key, Value> &processor, size_t max_producers, uint32_t delay): processor_(processor), max_producers_(max_producers), delay_(delay) {
		for (int i = 0; i < max_producers; i++) {
			//
			producers_.push_back(std::make_unique<Producer<Key, Value>>(processor_, i, 0, delay_));
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

	~ProducersBag() {
#ifdef DEBUG
		std::cout << "ProducersBag Destructor Call" << std::endl;
#endif // DEBUG

		
	}
private:
	size_t max_producers_;
	std::vector<std::unique_ptr<Producer<Key, Value>>> producers_;
	IEnqueue<Key, Value> &processor_;
	bool running_{ false };
	std::thread th_;
	uint32_t delay_;
};