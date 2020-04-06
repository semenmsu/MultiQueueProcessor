#pragma once
#include <memory>
#include "IConsumer.h"

template <typename Key, typename Value>
struct ISubscriber {
	virtual void Subscribe(Key id, std::shared_ptr<IConsumer<Key, Value>> consumer) = 0;
	virtual void Unsubscribe(Key id) = 0;
};
