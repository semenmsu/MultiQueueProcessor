#pragma once
#include <map>
#include <thread>
#include "Consumer.h"
#include "ISubscriber.h"
#include "utils.h"
#include <list>

template <typename Key, typename Value>
class ConsumersBag {
public:
	ConsumersBag(ISubscriber<Key, Value>& dispatcher, size_t max_consumers) :
		dispatcher_(dispatcher), max_consumers_(max_consumers), running_{ false } {
		for (int i = 0; i < max_consumers_; i++) {
			consumers_.emplace(i, make_consumer<Key, Value>(i));
		}
	}

	void Process() {
		while (running_) {
			Wait(1);
		}
	}

	void Analyze() {
		std::list<std::shared_ptr<Consumer<Key, Value>>> consumers;
		for (auto& kvp : consumers_) {
			consumers.push_back(kvp.second);
		}
		AnalyzeConsumers(consumers);
	}

	void Run() {
		try {

			for (int i = 0; i < max_consumers_; i++) {
				dispatcher_.Subscribe(i, consumers_[i]);
			}
			running_ = true;
			std::thread th(std::bind(&ConsumersBag::Process, this));
			th_ = std::move(th);
		}
		catch (const std::exception & ex) {
			std::cout << ex.what() << std::endl;
		}
	}

	void Stop() {
		running_ = false;
	}

	~ConsumersBag() {
		Stop();
		if (th_.joinable()) {
			th_.join();
		}
#ifdef DEBUG
		std::cout << "ConsumersBag Destructor Call" << std::endl;
#endif // DEBUG

		
	}
private:
	std::map<int32_t, std::shared_ptr<Consumer<Key, Value>>> consumers_;
	size_t max_consumers_;
	ISubscriber<Key, Value>& dispatcher_;
	bool running_;
	std::thread th_;
};