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

#ifndef SIMULATION_H_
#define SIMULATION_H_

//-----------------Instruction Instance Through Pipeline----------------------
/**
    Tracks the instruction instance as it goes through the pipeline
 */
struct PipelineInst {
    ElementQueueNode* trace_inst;                   // pointer to trace instruction
    uint64_t seq_num;                               // instruction number (to maintain program order)

    int ex_cycles_left;                             // remaining EX cycles
    int mem_cycles_left;                            // remaining MEM cycles

    // for data hazards
    int unresolved_deps;                            // number of unsatisfied dependences
    bool result_ready;                              // computation done
    bool entered_ex;                                // entered ex pipeline stage
    bool entered_mem;                               // entered mem pipeline stage

    std::vector<PipelineInst*> dependents;          // instructions waiting 
};


//-----------------Simulation Structure------------------------------------
/**
    Main simulation structure that tracks stats and pipeline states and handles instruction cycle
 */
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
            cumulative_store_inst = 0;

		};
		~Simulation() {
			delete ElementQ;
			for (PipelineInst* inst : all_insts) {
        		     delete inst;
    			}
		};

		// Simulation events (stages of pipeline)
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

			printf("===== Simulation Statistics =====\n");
            printf("Cycles = %d\n", simulation_clock);
            printf("Execution Time (ms) = %.6f\n", exec_time_ms);
            printf("Total retired instructions = %d\n", retired_count);

			printf("===== Instruction Histogram =====\n");
            if (retired_count > 0) {
                printf("Integer (%%) = %.4f\n", 100.0 * cumulative_integer_inst / retired_count);
                printf("FP (%%)      = %.4f\n", 100.0 * cumulative_fp_inst / retired_count);
                printf("Branch (%%)  = %.4f\n", 100.0 * cumulative_branch_inst / retired_count);
                printf("Load (%%)    = %.4f\n", 100.0 * cumulative_load_inst / retired_count);
                printf("Store (%%)   = %.4f\n", 100.0 * cumulative_store_inst / retired_count);
            } else {
                printf("Integer %% = 0.00\n");
                printf("FP %%      = 0.00\n");
                printf("Branch %%  = 0.00\n");
                printf("Load %%    = 0.00\n");
                printf("Store %%   = 0.00\n");
            }
		};

        void PrintInstructionWindow() {

            printf("====INSTRUCTION WINDOW FOR CYCLE %d====\n", simulation_clock);
            for (PipelineInst* inst : if_stage) {
                printf("IF INST %s\n", inst->trace_inst->program_counter.c_str());
            }
            for (PipelineInst* inst : id_stage) {
                printf("ID INST %s\n", inst->trace_inst->program_counter.c_str());
            }
            for (PipelineInst* inst : ex_stage) {
                printf("EX INST %s\n", inst->trace_inst->program_counter.c_str());
            }
            for (PipelineInst* inst: mem_stage) {
                printf("MEM INST %s\n", inst->trace_inst->program_counter.c_str());
            }
            for (PipelineInst* inst: wb_stage) {
                printf("WB INST %s\n", inst->trace_inst->program_counter.c_str());
            }
        }
    
	private:
        // Queues
		ElementQueue* ElementQ;     // Element Queue for all elements used in simulation

        // Arguments
		char* filename;				// Name of input trace file
        int start_inst;				// Instruction in the trace to start the simulation
		int inst_count;				// Number of instructions to simulate starting from start_inst
		int depth_config;			// Pipeline depth configuration

        int fetched_count;
        int retired_count;

        // Timer
        int simulation_clock;

		// Performance metrics
		int cumulative_execution;		// Accumulated execution time of completed instructions
		int cumulative_integer_inst;	// Accumulated number of retired integer instructions
		int cumulative_fp_inst;			// Accumulated number of retired floating point instructions
		int cumulative_branch_inst;		// Accumulated number of retired branch instructions
		int cumulative_load_inst;		// Accumulated number of retired load instructions
		int cumulative_store_inst;		// Accumulated number of retired store instructions
		
        // Flags
        bool fetch_stalled;
        bool resume_fetch_next_cycle;

        bool used_load_mem_port = false;
        bool used_store_mem_port = false;
        bool used_int_unit = false;
        bool used_fp_unit = false;
        bool used_branch_unit = false;

        uint64_t next_seq_num = 0;      // Next pipeline instance (counter)

        // Stages of pipeline
        std::deque<PipelineInst*> if_stage;
        std::deque<PipelineInst*> id_stage;
        std::deque<PipelineInst*> ex_stage;
        std::deque<PipelineInst*> mem_stage;
        std::deque<PipelineInst*> wb_stage;
	
        // Dependency tracking
        std::unordered_map<std::string, PipelineInst*> last_dynamic_pc;
        
        // Pipeline instances
        std::vector<PipelineInst*> all_insts;

        // Delays and speed
        int GetEXLatency(int inst_type) const;
        int GetMEMLatency(int inst_type) const;
        double GetFrequencyGHz() const;
        double GetExecutionTimeMs() const;

        // Helpers
        bool PipelineEmpty() const;
        void MarkDependenceSatisfied(PipelineInst* inst);
        PipelineInst* BuildFetchedInstruction(ElementQueueNode* src);
};


#endif

















































