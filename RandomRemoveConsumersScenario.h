#pragma once
#include "EventQueue.h"
#include "BruteForceQueue.h"
#include "NaiveQueue.h"
#include "MultiQueueProcessor.h"
#include "RandomRemoveConsumersBag.h"
#include "ProducersBag.h"


template<typename Value, typename Queue>
int RandomRemoveConsumersTest(uint32_t consumers_num, uint32_t delay_msk, std::string queue_name) {
	MultiQueueProcessor<int32_t, Value, Queue> mqp;
	ProducersBag<int32_t, Value> producers(mqp, consumers_num, 1);
	RandomRemoveConsumersBag<int32_t, Value> consumers(mqp, consumers_num, delay_msk);
	producers.Run();
	consumers.Run();
	Wait(NUM_SECS);
	consumers.Stop();
	SuperSleep(1000);
	consumers.Analyze();
	return 0;
}

template <typename Value>
void RandomRemoveConsumersScenario(uint32_t consumers_num, uint32_t delay_mks)
{
    std::cout << "================================================================" << std::endl;
    std::cout << "RandomSubscribeUnsusbscribeScenario               " << std::endl;
    std::cout << "Number Producers And Consumers " << consumers_num << std::endl;
    std::cout << "SizeOfValue = " << sizeof(Value) << "                    " << std::endl
        << std::endl;
	RandomRemoveConsumersTest<int32_t, BruteForceQueue<int32_t, Value>>(consumers_num, delay_mks, "BruteForceQueue");
    std::cout << "================================================================" << std::endl
        << std::endl;
}
