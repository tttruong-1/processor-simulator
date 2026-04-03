#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<float.h>
#include <cstdint>

#ifndef PROJ_H_
#define PROJ_H_


struct ElementQueueNode {
    double arrival_time;  // customer arrival time, measured from time t=0, inter-arrival times exponential
    double service_time;  // customer service time (exponential) 
};

struct EventQueueNode {
    double event_time; // event start time
    int event_type;   // Event type. 1: Fetch; 2: Decode; 3: ReadOperands
    ElementQueueNode* qnode;  // pointer to corresponding element in the Element Queue
    struct EventQueueNode *next;  // pointer to next event
};

class Simulation {
	public:
		Simulation(int seed_in, double lambda_in, double mu_in) {
			lambda = lambda_in;
			mu = mu_in;
			seed = seed_in;
		};
		~Simulation() {
			//delete ElementQ;
			//delete EventQ;
		};
		void FetchInstruction();
		void DecodeAndRead();
		void InstructionIssueAndExecute();
		void Memoryaccess();
		void WritebackResults();

		// This function should be called to print periodic and/or end-of-simulation statistics
		void PrintStatistics() {
			
		};
	private:
	// inputs
	double lambda;
	double mu;
	int seed;

	// queues
};

#endif