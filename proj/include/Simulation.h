#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<float.h>

#ifndef SIMULATION_H_
#define SIMULATION_H_


class Simulation {
	public:
		Simulation(char* filename_in, int start_inst_in, int inst_count_in, int depth_config_in) {

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
        

	// queues
};


#endif