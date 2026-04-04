#include <cstdint>

#ifndef ELEMENT_QUEUE_H_
#define ELEMENT_QUEUE_H_

struct ElementQueueNode {
    double arrival_time;  // customer arrival time, measured from time t=0, inter-arrival times exponential
    double service_time;  // customer service time (exponential) 
};


class ElementQueue {
	public:
        ElementQueue();
        ~ElementQueue();
        ElementQueueNode* GetCurrentElement();
        ElementQueueNode* GetElementAtIndex(uint64_t index);
        ElementQueueNode* AdvanceToNextElement();

    private:
        void InitializeQueue(int seed, double lambda, double mu, int total_departures);
        uint64_t size;  // total size of element queue
        uint64_t current; // Point to the current node being processed for arrival event
        ElementQueueNode* ElementArray;  // Array containing all elements, created at the beginning of simulation
};


#endif