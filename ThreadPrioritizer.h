#pragma once
#include <iostream>
#include "settings.h"

#ifdef LINUX
#include <pthread.h>
#endif // LINUX

class ThreadPrioritizer
{
public:
	ThreadPrioritizer(int32_t priority) : priority_(priority)
	{
#ifdef LINUX
		pthread_t thId = pthread_self();
		int policy = 0;
		int max_prio_for_policy = priority_;
		pthread_attr_init(&thAttr);
		pthread_attr_getschedpolicy(&thAttr, &policy);
		max_prio_for_policy = sched_get_priority_max(policy);
		pthread_setschedprio(thId, max_prio_for_policy);
#endif // Linux

		/**/
	}

	~ThreadPrioritizer()
	{
		//clean
#ifdef LINUX
		pthread_attr_destroy(&thAttr);
#endif // LINUX
	}

private:
#ifdef LINUX
	pthread_attr_t thAttr;
#endif // LINUX

	int32_t priority_;
};
