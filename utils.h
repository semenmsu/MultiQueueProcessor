#pragma once
#include <thread>
#include <chrono>
#include <type_traits>
#include "settings.h"

void SuperSleep(volatile uint32_t mks)
{
    std::this_thread::sleep_for(std::chrono::microseconds(mks));
}

//#define DEBUG

void Wait(int secs)
{

    SuperSleep(secs * 1'000'000);
}

template <class, class = std::void_t<>>
struct HasEmpty : std::false_type
{
};

template <class, class = std::void_t<>>
struct HasSize : std::false_type
{
};

template <class T>
struct HasEmpty<T,
                std::void_t<decltype(&T::empty)>> : std::true_type
{
};

template <class T>
struct HasSize<T,
               std::void_t<decltype(&T::size)>> : std::true_type
{
};

template <typename T>
bool constexpr is_iterable()
{
    if constexpr (HasEmpty<T>::value && HasSize<T>::value)
    {
        return true;
    }
    return false;
}

template <typename T>
void constexpr AnalyzeConsumers(T &consumers)
{
    //is_call_possible<Foo, void(double)>::value)

    int appr_msgs_in_sec = 0;
    int32_t errors_num = 0;
    int32_t errors_num_in_sec = 0;
    int64_t appr_total_receive_msgs_in_sec = 0;

    //if constexpr (HasBegin<T>::value)
    if constexpr (is_iterable<T>())
    {
        for (auto &consumer : consumers)
        {
            //AnalyzeConsumers(consumer);
            //PrintInfo(consumer);
            appr_msgs_in_sec += consumer.get()->GetLastSeqId() / NUM_SECS;
            appr_total_receive_msgs_in_sec += consumer.get()->GetTotalMessages() / NUM_SECS;

            errors_num += consumer.get()->GetNumErrors();
        }
    }
    else
    {
        appr_msgs_in_sec += consumers.get()->GetLastSeqId() / NUM_SECS;
        appr_total_receive_msgs_in_sec += consumers.get()->GetTotalMessages() / NUM_SECS;
        errors_num += consumers.get()->GetNumErrors();
    }

    errors_num_in_sec = errors_num / NUM_SECS;
    std::cout << "    approximately rec messages    : " << appr_total_receive_msgs_in_sec << " / sec" << std::endl;
    std::cout << "    approximately all messages    : " << appr_msgs_in_sec << " / sec" << std::endl;
    std::cout << "    approximately errors          : " << errors_num_in_sec << " / sec" << std::endl;
    std::cout << "    approximately errors total    : " << errors_num << std::endl;
}

struct Value64
{
    Value64()
    {
        value_ = 0;
    }

    Value64(int32_t value) : value_{value} {}

    int32_t value_{0};
    int32_t value1_{0};
    int64_t value2_{0};
    int64_t value3_{0};
    int64_t value4_{0};
    int64_t value5_{0};
    int64_t value6_{0};
    int64_t value7_{0};
    int64_t value8_{0};
};

struct Value128
{
    Value64 value_;
    Value64 value1_;

    Value128()
    {
    }

    Value128(int32_t value) : value_(value)
    {
    }
};

struct Value256
{
    Value128 value_;
    Value128 value1_;

    Value256()
    {
    }

    Value256(int32_t value) : value_(value)
    {
    }
};

struct Value512
{
    Value256 value_;
    Value256 value1_;

    Value512()
    {
    }

    Value512(int32_t value) : value_(value)
    {
    }
};

struct Value1024
{
    Value512 value_;
    Value512 value1_;

    Value1024()
    {
    }

    Value1024(int32_t value) : value_(value)
    {
    }
};
