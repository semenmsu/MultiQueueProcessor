#pragma once
//#define MaxCapacity 100
#define QUEUE_MAX_CAPACITY 100
#define QUEUE_PERSIST false
#define NUM_SECS 3
//#define LINUX
#define HIGH_PRIORITY -20
#define MEDIUM_PRIORITY 0
#define LOW_PRIORITY 19

#ifdef __unix__
#define LINUX
#elif defined(_WIN32) || defined(WIN32)
#define WINDOWS_OS
#endif
