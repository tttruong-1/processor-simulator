#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<float.h>

#include<deque>
#include<vector>
#include<string>
#include<unordered_map>

#include"ElementQueue.h"
#include"EventQueue.h"

#ifndef SIMULATION_H_
#define SIMULATION_H_

struct PipelineInst {
    ElementQueueNode* trace_inst;                   // pointer to trace instruction
    uint64_t seq_num;                               // instruction number
    int ex_cycles_left;                             // remaining EX cycles
    int mem_cycles_left;                            // remaining MEM cycles

    int unresolved_deps;                            // number of unsatisfied dependences
    bool result_ready;                              
    bool entered_ex;                                
    bool entered_mem;                               

    std::vector<PipelineInst*> dependents;          // instructions waiting 
};

class Simulation {
	public:
		Simulation(char* filename_in, int start_inst_in, int inst_count_in, int depth_config_in) {
			// Input arguments
			filename = filename_in;
			start_inst = start_inst_in;
			inst_count = inst_count_in;
			depth_config = depth_config_in;

			ElementQ = new ElementQueue(filename, start_inst, inst_count);
			// ElementQ->PrintElementQueue();	// For debug
			EventQ = nullptr;
			simulation_clock = 0;
            fetched_count = 0;
            retired_count = 0;

            fetch_stalled = false;
            resume_fetch_next_cycle = false;

            cumulative_execution = 0;
            cumulative_integer_inst = 0;
            cumulative_fp_inst = 0;
            cumulative_branch_inst = 0;
            cumulative_load_inst = 0;
            clumulative_store_inst = 0;

            simulated_stats[0] = simulated_stats[1] = simulated_stats[2] = 0.0;
		};
		~Simulation() {
			delete ElementQ;
			//delete EventQ;
			for (PipelineInst* inst : all_insts) {
        		     delete inst;
    			}
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
			double exec_time_ms = GetExecutionTimeMs();

			printf("\n===== Simulation Statistics =====\n");
            printf("Cycles = %d\n", simulation_clock);
            printf("Execution_time_ms = %.6f\n", exec_time_ms);
            printf("Retired_instructions = %d\n", retired_count);

            if (retired_count > 0) {
                printf("Integer %% = %.2f\n", 100.0 * cumulative_integer_inst / retired_count);
                printf("FP %%      = %.2f\n", 100.0 * cumulative_fp_inst / retired_count);
                printf("Branch %%  = %.2f\n", 100.0 * cumulative_branch_inst / retired_count);
                printf("Load %%    = %.2f\n", 100.0 * cumulative_load_inst / retired_count);
                printf("Store %%   = %.2f\n", 100.0 * clumulative_store_inst / retired_count);
            } else {
                printf("Integer %% = 0.00\n");
                printf("FP %%      = 0.00\n");
                printf("Branch %%  = 0.00\n");
                printf("Load %%    = 0.00\n");
                printf("Store %%   = 0.00\n");
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
		
		int fetched_count;
        int retired_count;

        bool fetch_stalled;
        bool resume_fetch_next_cycle;

        uint64_t next_seq_num = 0;

        std::deque<PipelineInst*> if_stage;
        std::deque<PipelineInst*> id_stage;
        std::deque<PipelineInst*> ex_stage;
        std::deque<PipelineInst*> mem_stage;
        std::deque<PipelineInst*> wb_stage;
	
        std::unordered_map<std::string, PipelineInst*> last_dynamic_pc;
        
        std::vector<PipelineInst*> all_insts;

        int GetEXLatency(int inst_type) const;
        int GetMEMLatency(int inst_type) const;
        double GetFrequencyGHz() const;
        double GetExecutionTimeMs() const;

        bool PipelineEmpty() const;
        void MarkDependenceSatisfied(PipelineInst* inst);
        PipelineInst* BuildFetchedInstruction(ElementQueueNode* src);
	// queues
};


#endif

















































