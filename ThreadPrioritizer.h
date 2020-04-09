#pragma once
#include <iostream>
#include "settings.h"

#ifdef LINUX
#include <pthread.h>
#endif // LINUX
#ifdef WINDOWS_OS
#include <windows.h>
#include <tchar.h>
#endif // WINDOWS_OS


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

#ifdef WINDOWS_OS
		DWORD dwError, dwThreadPri;
		if (priority_ == HIGH_PRIORITY) {
			if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
				_tprintf(TEXT("Failed to end REALTIME_PRIORITY_CLASS mode (%d)\n"), GetLastError());
			}
			else {
				dwThreadPri = GetThreadPriority(GetCurrentThread());
				_tprintf(TEXT("Current thread priority is 0x%x\n"), dwThreadPri);
			}
		}
#endif // WINDOWS_OS


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


