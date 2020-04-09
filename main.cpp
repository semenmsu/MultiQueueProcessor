// Mayers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "RandomRemoveConsumersBag.h"
#include "RandomSubscribeUnsubscribeScenario.h"
#include "SimpleProducerConsumerScenario.h"
#include "ExtraManyProducerConsumerScenario.h"
#include "RandomRemoveConsumersScenario.h"

int main()
{
	//
	SimpleProducersConsumersScenario<int32_t>(10, 20, 5);
	SimpleProducersConsumersScenario<Value64>(10, 20, 5);
	ExtraManyProducersConsumersScenario<int32_t>(150, 10);
	ExtraManyProducersConsumersScenario<int32_t>(400, 100);
	ExtraManyProducersConsumersScenario<Value64>(150, 10);
	RandomSubscribeUnsusbscribeScenario<int32_t>(100, 100);
	RandomRemoveConsumersScenario<int32_t>(100, 1'000);
}
