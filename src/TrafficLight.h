#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <chrono>
#include <stdlib.h>
#include <thread>
#include "TrafficObject.h"

using namespace std::chrono;

// forward declarations to avoid include cycle
class Vehicle;


template <class T>
class MessageQueue
{
public:
    void send(T&& msg);
    T receive();
    
private:
    std::mutex MsgQ_mtx;
    std::condition_variable cv;  
    std::deque <T>_queue;  
};


enum TrafficLightPhase{
    red,
    green
};

class TrafficLight: public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate() override;
    

private:
    // typical behaviour methods
    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _phases;
    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
};

#endif