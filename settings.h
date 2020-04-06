#pragma once
#define MaxCapacity 500
#define Persist false
#define NUM_SECS 3
//#define LINUX
#define HIGH_PRIORITY -20
#define MEDIUM_PRIORITY 0
#define LOW_PRIORITY 19

#ifdef __unix__
#define LINUX
#elif defined(_WIN32) || defined(WIN32)
#endif
