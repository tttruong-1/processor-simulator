#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<float.h>

#include"ElementQueue.h"
#include"EventQueue.h"

#ifndef SIMULATION_H_
#define SIMULATION_H_

class Simulation {
	public:
		Simulation(char* filename_in, int start_inst_in, int inst_count_in, int depth_config_in) {
			// Input arguments
			filename = filename_in;
			start_inst = start_inst_in;
			inst_count = inst_count_in;
			depth_config = depth_config_in;

		};
		~Simulation() {
			//delete ElementQ;
			//delete EventQ;
		};

		// Simulation events
		void FetchInstruction();
		void DecodeAndRead();
		void InstructionIssueAndExecute();
		void Memoryaccess();
		void WritebackResults();

		// Main simulation loop
		void RunSimulation();

		// This function should be called to print periodic and/or end-of-simulation statistics
		void PrintStatistics() {
			printf("Printing statistics\n");
			for (int i = 0; i < 5; i++){
				printf("\t%d: Hello world.\n",i);
			}
		};
	private:
		ElementQueue* ElementQ;     // Element Queue for all elements used in simulation
		EventQueue* EventQ;         // Event Queue for events to be scheduled 	

		char* filename;				// Name of input trace file
        int start_inst;				// Instruction in the trace to start the simulation
		int inst_count;				// Number of instructions to simulate starting from start_inst
		int depth_config;			// Pipeline depth configuration

		double simulated_stats[3]; 

		int simulation_clock;

		// Performance metrics
		int cumulative_execution;		// Accumulated execution time of completed instructions
		int cumulative_integer_inst;	// Accumulated number of retired integer instructions
		int cumulative_fp_inst;			// Accumulated number of retired floating point instructions
		int cumulative_branch_inst;		// Accumulated number of retired branch instructions
		int cumulative_load_inst;		// Accumulated number of retired load instructions
		int clumulative_store_inst;		// Accumulated number of retired store instructions

	// queues
};


#endif