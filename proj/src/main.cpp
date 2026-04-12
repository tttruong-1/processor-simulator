#include<iostream>
#include<filesystem>

#include"Simulation.h"


//-----------------Main------------------------------------
/**
	Execution:
	./proj "trace file name" "line of instruction to start simulation" "total instructions to run" "pipeline depth (1-4)"

	Example:
	./proj srv_0 200000 100000 2
*/
int main(int argc, char* argv[]){

	// Input arguments
	if(argc >= 5){
		char* trace_file_name = argv[1];		// filename of trace
		int start_inst = atoi(argv[2]);			// first instruction of trace to simulate
		int inst_count = atoi(argv[3]);			// total number of instructions to run
		int D = atoi(argv[4]);					// Pipeline depth configuration D = 1, 2, 3, 4
		
	   	// Error checks for input variables, exit with exit code 1 if input is incorrect
		if ( !std::filesystem::exists(trace_file_name) ) { // no trace file found with trace_file_name
			printf("Cannot locate trace file: %s. Terminating Simulation...\n", trace_file_name);
			return 1;
		}
		else if ( D < 0 || D > 4 ) { // pipline is configured to have a depth between 1-4
			printf("Invalid pipeline depth configuration: %d. Terminating Simulation...\n", D);
			return 1;
		}
		else if (start_inst < 0 || inst_count < 0){ // instruction line or number of total instructions are positive whole numbers
			printf("Input Error. Terminating Simulation...\n");
			return 1;
		}

   		// If no input errors, create Simulation s
   		Simulation* s = new Simulation(trace_file_name, start_inst, inst_count, D);

   		// Start Simulation
		printf("Simulating Simple Processor Pipeline with: \n- trace_file = %s \n- start_inst = %d \n- inst_count = %d \n- D = %d\n", trace_file_name, start_inst, inst_count, D);
		s->RunSimulation();
		s->PrintStatistics(); // final calculated statistics

		// Delete s
		delete s;
		return 0;
	}
	else {
		printf("Insufficient number of arguments provided!\n");
		return 1;
	}
}
