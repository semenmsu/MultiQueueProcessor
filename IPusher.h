#pragma once

template <typename Key, typename Value>
class IPusher {
public:
	virtual bool TryPush(Key id, Value& value) = 0;
};