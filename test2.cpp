#include "ExtraManyProducerConsumerScenario.h"
#include "utils.h"

int main()
{
    ExtraManyProducersConsumersScenario<int32_t>(150, 10);
    ExtraManyProducersConsumersScenario<Value64>(150, 10);
    return 0;
}