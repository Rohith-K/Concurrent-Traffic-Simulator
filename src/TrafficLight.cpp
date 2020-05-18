#include <iostream>
#include <random>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> ulock(MsgQ_mtx);
    cv.wait(ulock, [this]{return !_queue.empty();});

    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> ulock(MsgQ_mtx);
    _queue.push_back(std::move(msg));
    cv.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _type = trafficLight;
}

void TrafficLight::waitForGreen()
{
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase msg = _phases.receive();
        if(msg == TrafficLightPhase::green)
            return;
    }

}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::random_device dev;
    std::mt19937 eng(dev());
    std::uniform_int_distribution<> dist46(4000,6000);

    // generate random cycle time
    auto cycle_time = dist46(eng);

    auto t2 = system_clock::now();

    while(true){
        
        auto t1 = system_clock::now();
        long time_span = duration_cast<milliseconds>(t1-t2).count();
        
        if(time_span >= cycle_time){
            _currentPhase = (_currentPhase==red?green:red);
            
            TrafficLightPhase cpy = _currentPhase;
            _phases.send(std::move(_currentPhase));
            t2 = system_clock::now();

            // generate another random cycle time
            cycle_time = dist46(eng);
        }
        // to reduce cpu overhead
        std::this_thread::sleep_for(milliseconds(1));
    }
   


}

