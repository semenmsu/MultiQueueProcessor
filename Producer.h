#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include "IEnqueue.h"
#include "utils.h"

template <typename Key, typename Value>
class Producer {
public:
	Producer(IEnqueue<Key, Value>& producer, Key key, Value start, uint32_t mks_delay) : 
		producer_(producer), key_{ key }, value_{ start }, mks_delay_{ mks_delay } {}

	Producer(const Producer& other) = delete;
	Producer(Producer&& other) = delete;

	void Process() {
		while (running_) {
			SuperSleep(mks_delay_);
			int32_t* value = (int32_t*)&value_;
			(*value)++;
			producer_.Enqueue(key_, value_);
		}
	}

	void Run() {
		try
		{

			std::thread th(std::bind(&Producer::Process, this));
			th_ = std::move(th);
			running_ = true;
		}
		catch (const std::exception & ex) {
			std::cout << "producer  id: " << key_ << std::endl;
			std::cout << ex.what() << std::endl;
		}
	}

	void Stop() {
		running_ = false;
	}

	~Producer() {
		Stop();
		if (th_.joinable()) {
			th_.join();
		}

#ifdef DEBUG
		std::cout << "Producer Destructor Call" << std::endl;
#endif // DEBUG

	}

private:
	IEnqueue<Key, Value> & producer_;
	std::thread th_;
	Key key_;
	Value value_;
	uint32_t mks_delay_;
	bool running_{ false };
};