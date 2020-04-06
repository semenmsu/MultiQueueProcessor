#pragma once

template <typename Key, typename Value>
struct IEnqueue {
	virtual void Enqueue(Key id, Value& value) = 0;
};
