#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include "IEnqueue.h"
#include "utils.h"
#include <vector>

//producer [from, to)
template <typename Key, typename Value>
class MultiProducer {
public:
	
	struct ProducerClient {
		Key Id;
		Value SeqId;

		ProducerClient(Key id, Value value): Id(id), SeqId(value){
			//std::cout << "ProducerClient with id " << id << std::endl;
		}

		void Update() {
			int32_t* value = (int32_t*)&SeqId;
			(*value)++;
		}
	};

	MultiProducer(IEnqueue<Key, Value>& producer, Key from, Key to,  Value start, uint32_t mks_delay) :
		producer_(producer), from_{ from }, to_{ to }, value_{ start }, mks_delay_{ mks_delay } {
		auto id = from_;
		while (id < to_) {
			ProducerClient client(id, value_);
			clients_.push_back(client);
			id++;
		}
	}

	MultiProducer(const MultiProducer& other) = delete;
	MultiProducer(MultiProducer&& other) = delete;

	void Process() {
		while (running_) {
			SuperSleep(mks_delay_);
			for (auto& client : clients_) {
				client.Update();
				producer_.Enqueue(client.Id, client.SeqId);
			}
		}
	}

	void Run() {
		try
		{
			std::thread th(std::bind(&MultiProducer::Process, this));
			th_ = std::move(th);
			running_ = true;
		}
		catch (const std::exception & ex) {
			//std::cout << "producer  id: " << key_ << std::endl;
			std::cout << ex.what() << std::endl;
		}
	}

	void Stop() {
		running_ = false;
	}

	~MultiProducer() {
		Stop();
		if (th_.joinable()) {
			th_.join();
		}

#ifdef DEBUG
		std::cout << "Producer Destructor Call" << std::endl;
#endif // DEBUG

	}

private:
	IEnqueue<Key, Value>& producer_;
	std::thread th_;
	Key from_;
	Key to_;
	Value value_;
	uint32_t mks_delay_;
	std::vector<ProducerClient> clients_;
	bool running_{ false };
};
