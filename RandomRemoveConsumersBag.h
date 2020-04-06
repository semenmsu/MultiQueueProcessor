#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <random>
#include <list>
#include "Consumer.h"
#include "ISubscriber.h"
#include "settings.h"
#include "utils.h"
#include <map>
#include <functional>

template <typename Key, typename Value>
class RandomRemoveConsumersBag
{
public:
    //(ISubscriber<Key, Value>& dispatcher, size_t max_consumers)
    RandomRemoveConsumersBag(ISubscriber<Key, Value> &dispatcher, size_t max_consumers, int32_t delay) : dispatcher_(dispatcher), max_consumers_(max_consumers), delay_(delay)
    {
        /*for (int i = 0; i < max_consumers_; i++)
        {
            consumers_.emplace(i, std::make_shared<Consumer<Key, Value>>(i));
        }*/
    }

    void Run()
    {
        running_ = true;
        std::thread th(std::bind(&RandomRemoveConsumersBag::Process, this));
        th_ = std::move(th);
    }

    void Process()
    {
        std::vector<int32_t> unsubscribed;
        std::vector<int32_t> subscribed;
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> uniform_real(0.0, 1.0);
        std::uniform_int_distribution<> uniform_int(0, max_consumers_ - 1);
        double subscribe_probability = 0.5;
        double unsubscribe_probability = 0.3;

        for (int i = 0; i < max_consumers_; i++)
        {
            unsubscribed.push_back(i);
        }
        while (running_)
        {
            //try subscribe with probability p
            if (subscribed.size() < max_consumers_ && uniform_real(gen) < subscribe_probability)
            {
                std::uniform_int_distribution<> subscribe_distribution(0, unsubscribed.size() - 1);
                auto index = subscribe_distribution(gen);
                auto value = unsubscribed[index];
                //std::cout << "size" << unsubscribed.size() << std::endl;
                //std::cout << "index" << index << std::endl;
                //std::cout << "subscribe " << value << std::endl;
                unsubscribed.erase(unsubscribed.begin() + index);

                subscribed.push_back(value);
                consumers_.emplace(value, std::make_shared<Consumer<Key, Value>>(value));
                dispatcher_.Subscribe(value, consumers_[value]);
            }

            //try unsubscribe with probability q
            if (subscribed.size() > 0 && uniform_real(gen) < unsubscribe_probability)
            {
                std::uniform_int_distribution<> unsubscribe_distribution(0, subscribed.size() - 1);
                auto index = unsubscribe_distribution(gen);
                auto value = subscribed[index];
                subscribed.erase(subscribed.begin() + index);
                unsubscribed.push_back(value);
                //dispatcher_.Unsubscribe(value);
                consumers_.erase(value);
                total_removes_++;
            }

            /*std::cout << "subscribed: ";
            for (auto &s : subscribed)
            {
                std::cout << s << " ";
            }
            std::cout << std::endl;
            std::cout << std::endl;*/
            //usleep(delay_);
            SuperSleep(delay_);
        }
    }

    void Analyze()
    {
        std::cout << "Total Removes: " << total_removes_ << std::endl;
        std::list<std::shared_ptr<Consumer<Key, Value>>> consumers;
        for (auto &kvp : consumers_)
        {
            consumers.push_back(kvp.second);
        }
        AnalyzeConsumers(consumers);
    }

    void Stop()
    {
        running_ = false;
    }

    ~RandomRemoveConsumersBag()
    {
        Stop();
        if (th_.joinable())
        {
            th_.join();
        }
    }

private:
    std::map<int32_t, std::shared_ptr<Consumer<Key, Value>>> consumers_;
    int32_t max_consumers_{0};
    ISubscriber<Key, Value> &dispatcher_;
    size_t total_removes_{0};
    bool running_{false};
    std::thread th_;
    int32_t delay_;
};
