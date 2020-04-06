#pragma once
#include "IConsumer.h"
#include <cassert>
#include <iostream>

template <typename Key, typename Value>
class Consumer : public IConsumer<Key, Value>
{
public:
	Consumer(Key id):id_(id){
		//std::cout << "Call Consumer Constructor " << id_ << std::endl;
	}

	void Consume(Key id, const Value& value) override  {

		total_messages_++;
		
		int32_t msq_seqid = *((int32_t*)&value);
#ifdef DEBUG
		//std::cout << "Consumer " << id << " receive message with seqid " << msq_seqid << std::endl;
#endif // DEBUG
		assert(id == id_); //check correct queue id
		assert(msq_seqid > seqid_); //check correct messages order
		if ((msq_seqid - seqid_) != 1) {

#ifdef DEBUG
			//std::cout << "[Error] Consumer id =" << id << "  seqid=" << msq_seqid << "  prev_seqid=" << seqid_ << std::endl;
#endif // DEBUG
			num_errors_++;
		}
		seqid_ = msq_seqid;
	}

	int32_t GetLastSeqId() {
		return seqid_;
	}

	int32_t GetTotalMessages() {
		return total_messages_;
	}

	int32_t GetNumErrors() {
		return num_errors_;
	}

	~Consumer() {
		//std::cout << "Consumer Destructor Call" << id_ << std::endl;
#ifdef DEBUG
		std::cout << "Consumer Destructor Call" << std::endl;
#endif // DEBUG

		
	}

private:
	int32_t seqid_{ 0 };
	int32_t num_errors_{ 0 }; //the number of holes in the data
	int32_t total_messages_{ 0 };
	Key id_;

};

//template< class T, class... Args >
//shared_ptr<T> make_shared(Args&&... args);
//auto consumer = std::make_shared<Consumer<int32_t, int32_t>>(1);
template<typename Key, typename Value, typename... Args>
std::shared_ptr<Consumer<Key, Value>> make_consumer(Args&&... args) {
	return std::make_shared<Consumer<Key, Value>>(std::forward<Args>(args)...);
}