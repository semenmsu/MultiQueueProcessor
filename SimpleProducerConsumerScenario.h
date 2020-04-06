#pragma once
#include <iostream>
#include "EventQueue.h"
#include "BruteForceQueue.h"
#include "NaiveQueue.h"
#include "ConsumersBag.h"
#include "ProducersBag.h"
#include "MultiQueueProcessor.h"

template <typename Value, typename Queue>
void SimpleProducersConsumersTest(int32_t consumers_num, int32_t producers_num, std::string queue_name)
{
	printf("    ******************** new scenario  producers=%d  consumers=%d ******************** \n", producers_num, consumers_num);
	printf("    %s\n", queue_name.c_str());
	MultiQueueProcessor<int32_t, Value, Queue> multiQueueProcessor;
	ProducersBag<int32_t, Value> producers(multiQueueProcessor, producers_num, 1);
	ConsumersBag<int32_t, Value> consumers(multiQueueProcessor, consumers_num);
	Wait(1);
	producers.Run();
	consumers.Run();
	Wait(NUM_SECS);
	consumers.Stop();
	Wait(1);
	consumers.Analyze();
}

template <typename Value>
void SimpleProducersConsumersScenario(int32_t from, int32_t to, int32_t step)
{
	std::cout << "================================================================" << std::endl;
	std::cout << "SimpleProducersConsumersScenario               " << std::endl;
	std::cout << "SizeOfValue = " << sizeof(Value) << "                    " << std::endl
			  << std::endl;

	for (int i = from; i <= to; i += step)
	{
		SimpleProducersConsumersTest<Value, BruteForceQueue<int32_t, Value>>(i, i, "BruteForceQueue");
		Wait(2);
		SimpleProducersConsumersTest<Value, NaiveQueue<int32_t, Value>>(i, i, "NaiveQueue");
		Wait(2);
		SimpleProducersConsumersTest<Value, EventQueue<int32_t, Value>>(i, i, "EventQueue");
		Wait(2);
		std::cout << "\n"
				  << std::endl;
	}
	std::cout << "================================================================" << std::endl
			  << std::endl;
}
