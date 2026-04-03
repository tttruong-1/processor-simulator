#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<float.h>
#include"proj.h"

//-----------------Functions------------------------------------
void FetchInstruction() {

}

void DecodeAndRead() {

}

void InstructionIssueAndExecute() {

}

void Memoryaccess() {

}

void WritebackResults() {

}


//-----------------Main------------------------------------
int main(int argc, char* argv[]){

	// input arguments lambda, mu
	if(argc >= 3){

		double lambda = atof(argv[1]);
		double mu = atof(argv[2]);
   
	   	// Add error checks for input variables here, exit with exit code 1 if input is incorrect
		if (lambda <= 0) {
			printf("lambda must be > 0. Terminating Simulation...\n");
			exit(1);
		}
		else if (mu <= 0) {
			printf("mu must be > 0. Terminating Simulation...\n");
			exit(1);
		}

   		// If no input errors, generate M/M/m/B computed statistics based on formulas from class
   		Simulation* s = new Simulation(random_seed, lambda, mu);

   		// Start Simulation
		printf("Simulating M/M/m/B queue with lambda = %f, mu = %f, S = %d\n", 
			lambda, mu, random_seed); 

		s->RunSimulation();
		s->PrintStatistics(total_departures, print_period, lambda);

		delete s;
		return 0;
	}
	else {
		printf("Insufficient number of arguments provided!\n");
		return 1;
	}
}
