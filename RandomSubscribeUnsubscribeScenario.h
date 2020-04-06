#pragma once
#include <iostream>
#include "RandomSubscribeConsumersBag.h"
#include "ProducersBag.h"
#include "MultiQueueProcessor.h"

template <typename Value>
void RandomSubscribeUnsusbscribeScenario(int32_t consumers_num, int32_t producer_delay)
{
    std::cout << "================================================================" << std::endl;
    std::cout << "RandomSubscribeUnsusbscribeScenario               " << std::endl;
    std::cout << "Number Producers And Consumers " << consumers_num << std::endl;
    std::cout << "SizeOfValue = " << sizeof(Value) << "                    " << std::endl
        << std::endl;
    MultiQueueProcessor<int32_t, Value> multiQueueProcessor;
    RandomSubscribeConsumersBag<int32_t, Value> consumers(multiQueueProcessor, consumers_num, 10'000);
    {
        ProducersBag<int32_t, Value> producers(multiQueueProcessor, consumers_num, producer_delay);
        Wait(1);
        producers.Run();
        consumers.Run();
        Wait(NUM_SECS);
        consumers.Stop();
    }
    Wait(0);
    consumers.Analyze();
    std::cout << "================================================================" << std::endl
        << std::endl;
}
