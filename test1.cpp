#include "SimpleProducerConsumerScenario.h"
#include "utils.h"

int main()
{
    SimpleProducersConsumersScenario<int32_t>(20, 100, 5);
    SimpleProducersConsumersScenario<Value64>(10, 20, 5);
    return 0;
}