#include "ElementQueue.h"

#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

struct EventQueueNode {
    double event_time; // event start time
    int event_type;   // Event type. 1: Fetch; 2: Decode; 3: ReadOperands
    ElementQueueNode* qnode;  // pointer to corresponding element in the Element Queue
    struct EventQueueNode *next;  // pointer to next event
};

class EventQueue {
	public:
		EventQueue() {}

		~EventQueue() {}

        
    private:
};


#endif