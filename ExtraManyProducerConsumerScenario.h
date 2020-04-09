#pragma once
#include "MultiQueueProcessor.h"
#include "MultiProducersBag.h"
#include "ConsumersBag.h"
#include "EventQueue.h"
#include "BruteForceQueue.h"
#include "NaiveQueue.h"
#include "LockFreeQueue.h"

template <typename Value, typename Queue>
int ExtraManyProducersTest(int num_threads, int num_producers, std::string queue_name)
{
	//std::cout << "ExtraManyProducersBruteForceQueueTest num_threads = " << num_threads << ", num_producers in thread = " << num_producers << std::endl;
	printf("    ******************** new scenario  num_threads=%d  producers_in_thread=%d ******************** \n", num_threads, num_producers);
	printf("    %s\n", queue_name.c_str());
	MultiQueueProcessor<int32_t, Value, Queue> mqp;
	MultiProducersBag<int32_t, Value> producers(mqp, num_threads, num_producers, 1);
	ConsumersBag<int32_t, Value> consumers(mqp, num_threads * num_producers);
	producers.Run();
	consumers.Run();
	Wait(NUM_SECS);
	consumers.Stop();
	Wait(1);
	consumers.Analyze();
	std::cout << std::endl;
	return 0;
}

template <typename Value>
void ExtraManyProducersConsumersScenario(int num_threads, int num_producers)
{
	std::cout << "================================================================" << std::endl;
	std::cout << "ExtraManyProducersConsumersScenario               " << std::endl;
	std::cout << "SizeOfValue = " << sizeof(Value) << "                    " << std::endl
			  << std::endl;
	ExtraManyProducersTest<Value, BruteForceQueue<int32_t, Value>>(num_threads, num_producers, "BruteForceQueue");
	Wait(2);
	ExtraManyProducersTest<Value, NaiveQueue<int32_t, Value>>(num_threads, num_producers, "NaiveQueue");
	Wait(2);
	ExtraManyProducersTest<Value, EventQueue<int32_t, Value>>(num_threads, num_producers, "EventQueue");
	Wait(2);
	ExtraManyProducersTest<Value, EventQueue<int32_t, Value, LockFreeQueue<int32_t, Value>>>(num_threads, num_producers, "EventQueueLockFree");
	Wait(2);
	std::cout << "================================================================" << std::endl
			  << std::endl
			  << std::endl;
}
