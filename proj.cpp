#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<float.h>
#include<string.h>
#include<iostream>
#include<filesystem>

#include"proj.h"

//-----------------Functions------------------------------------


//-----------------Main------------------------------------
int main(int argc, char* argv[]){

	// input arguments lambda, mu
	if(argc >= 4){
		char* trace_file_name = argv[1];		// filename of trace
		int start_inst = atoi(argv[2]);			// first instruction of trace to simulate
		int inst_count = atoi(argv[3]);			// total number of instructions to run
		int D = atoi(argv[4]);					// Pipeline depth configuration D = 1, 2, 3, 4
   
	   	// Add error checks for input variables here, exit with exit code 1 if input is incorrect
		if ( !std::filesystem::exists(trace_file_name) ) {
			printf("Cannot locate trace file: %s\n", trace_file_name);
			return 1;
		}
		if ( D < 0 || D > 4 ) {
			printf("Invalid pipeline depth configuration: %d\n", D);
			return 1;
		}

   		// If no input errors, create processor simulation s
   		// Simulation* s = new Simulation(trace_file_name, start_inst, inst_count, D);

   		// Start Simulation
		printf("Simulating simple processor with trace_file = %s, start_inst = %d, inst_count = %d, depth_config = %d\n...", trace_file_name, start_inst, inst_count, D);
		// s->RunSimulation();
		// s->PrintStatistics();

		// delete s;
		return 0;
	}
	else {
		printf("Insufficient number of arguments provided!\n");
		return 1;
	}
}
