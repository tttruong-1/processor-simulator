#include "Simulation.h"
#include "instType.h"


//-----------------Simulation Delays and Execution Times--------------------

/**
Return EX cycle count for INT/FP/BRANCH instructions
*/
int Simulation::GetEXCycleCount(int inst_type) const {
    // D=2 and D=4: FP spends 2 cycles in EX
    if ((depth_config == 2 || depth_config == 4) && inst_type == FLOATING_POINT) {
        return 2;
    }
    return 1;
}

/**
Return MEM cycle count for STORE/LOAD instructions
*/
int Simulation::GetMEMCycleCount(int inst_type) const {
    // D=3 and D=4: Loads spend 3 cycles in MEM
    if ((depth_config == 3 || depth_config == 4) && inst_type == LOAD) {
        return 3;
    }
    return 1;
}

/**
Return frequency based on D (GHz)
*/
double Simulation::GetFrequency() const {
    switch (depth_config) {
        case 1: return 1.0;
        case 2: return 1.2;
        case 3: return 1.7;
        case 4: return 1.8;
        default: return 1.0;
    }
}

/**
Return execution time (ms)
*/
double Simulation::GetExecutionTime() const {
    // time in ms = cycles 
    return (double)simulation_clock / (GetFrequency() * 1000000.0);
}


//-----------------Simulation Helpers--------------------------------
bool Simulation::PipelineEmpty() const {
    return if_stage.empty() && id_stage.empty() && ex_stage.empty() &&
           mem_stage.empty() && wb_stage.empty();
}


void Simulation::MarkDependenceSatisfied(PipelineInst* inst) {
    // If instruction not found or it's result has been set to ready, 
    if (inst == nullptr || inst->is_resolved) {
        return;
    }

    // Set current instruction to resolved
    inst->is_resolved = true;

    // for all instruction dependent on this instruction, remove this dependency from their unresolved dependency count
    for (PipelineInst* dependent : inst->dependents) {
        if (dependent != nullptr && dependent->unresolved_deps > 0) {
            dependent->unresolved_deps--;
        }
    }
}

/**
    Makes the pipeline instance (fetches instruction)
*/
PipelineInst* Simulation::BuildFetchedInstruction(ElementQueueNode* src) {
    if (src == nullptr) {
        return nullptr;
    }

    // Initialize new pipeline instruction
    PipelineInst* inst = new PipelineInst;
    inst->trace_inst = src;
    inst->seq_num = next_seq_num++;
    inst->ex_cycles_left = GetEXCycleCount(src->inst_type);
    inst->mem_cycles_left = GetMEMCycleCount(src->inst_type);
    inst->unresolved_deps = 0;
    inst->is_resolved = false;
    inst->entered_ex = false;
    inst->entered_mem = false;

    all_insts.push_back(inst);

    // dependency is on the last dynamic instance of that PC seen so far.
    for (const std::string& dep_pc : src->dependences) {
        // Find the last instance of the instruction dependence 
        auto it = last_dynamic_pc.find(dep_pc);

        if (it != last_dynamic_pc.end() && it->second != nullptr) {
            // If the dependency is found after start_inst,
            // and this dependency is not yet is_resolved/completed,
            // add it to the list of instructions dependent on this dependency
            PipelineInst* producer = it->second;
            if (!producer->is_resolved) {
                inst->unresolved_deps++;
                producer->dependents.push_back(inst);
            }
        }
        // if not found, dependency was before start_inst so we ignore
    }

    // becomes the newest dynamic instance for its PC
    last_dynamic_pc[src->program_counter] = inst;

    return inst;
}


//-----------------Simulation Pipeline Stages-----------------------------
void Simulation::FetchInstruction() {
    if (fetch_stalled) {
        return;
    }

    // IF is 2-wide
    while ((int)if_stage.size() < 2 && fetched_count < inst_count) {
        // Get instruction from ElementQueue
        ElementQueueNode* src = ElementQ->GetElementAtIndex((uint64_t)fetched_count);
        if (src == nullptr) {
            return;
        }

        // Create pipeline instruction from fetched instruction
        PipelineInst* inst = BuildFetchedInstruction(src);
        if_stage.push_back(inst);
        fetched_count++;

        // once a branch is fetched, stop fetching following instructions
        if (src->inst_type == BRANCH) {
            fetch_stalled = true;
            break;
        }
    }
}

void Simulation::DecodeAndRead() {
    // Move from IF to ID, up to width 2 total in ID
    while (!if_stage.empty() && (int)id_stage.size() < 2) {
        PipelineInst* inst = if_stage.front();
        if_stage.pop_front();
        id_stage.push_back(inst);
    }
}

void Simulation::InstructionIssueAndExecute() {
    // Advance instructions already in EX
    for (PipelineInst* inst : ex_stage) {
        if (inst->ex_cycles_left > 0) {
            inst->ex_cycles_left--;
        }
    }

    // Then move completed EX instructions to MEM in-order
    while (!ex_stage.empty() && mem_stage.size() < 2) {
        PipelineInst* inst = ex_stage.front();

        // in-order leaving EX
        if (inst->ex_cycles_left > 0) {
            break;
        }

        int type = inst->trace_inst->inst_type;

        // Check if LOAD/STORE instruction can be started (stop adding to MEM stage if cannot proceed)
        if (type == LOAD) {
            if (used_load_mem_port) break;
            used_load_mem_port = true;
        }
        if (type == STORE) {
            if (used_store_mem_port) break;
            used_store_mem_port = true;
        }

        // Remove from EX state and add to MEM stage
        ex_stage.pop_front();
        inst->entered_mem = true;
        mem_stage.push_back(inst);
        
        // Update dependency map if completed instruction is INT, FP, BRANCH
        if (type == INTEGER) {
            MarkDependenceSatisfied(inst);
            used_int_unit = false;
        }
        else if (type == FLOATING_POINT){
            MarkDependenceSatisfied(inst);
            used_fp_unit = false;
        } 
        else if (type == BRANCH) {
            MarkDependenceSatisfied(inst); 
            resume_fetch_next_cycle = true; // Branch resolves after EX completes.
            used_branch_unit = false;
        }
    }

    // Issue from ID to EX

    while (!id_stage.empty() && (int)ex_stage.size() < 2) {
        PipelineInst* inst = id_stage.front();
        int type = inst->trace_inst->inst_type;

        // Must wait for data dependences
        if (inst->unresolved_deps > 0) {
            break; // later instructions in ID cannot pass this one
        }

        // Structural hazards for EX 
        if ((type == INTEGER && used_int_unit) ||
            (type == FLOATING_POINT && used_fp_unit) ||
            (type == BRANCH && used_branch_unit)) {
                break;
            }

        id_stage.pop_front();
        inst->entered_ex = true;
        ex_stage.push_back(inst);

        // Update execution type units in use
        if (type == INTEGER) 
            used_int_unit = true;

        else if (type == FLOATING_POINT) 
            used_fp_unit = true;

        else if (type == BRANCH) 
            used_branch_unit = true;
    }
}

void Simulation::Memoryaccess() {
    // Advance instructions already in MEM if require >1 cycle to finish
    for (PipelineInst* inst : mem_stage) {
        if (inst->mem_cycles_left > 0) {
            inst->mem_cycles_left--;
        }
    }

    // Move completed MEM instructions to WB in-order
    while (!mem_stage.empty() && (int)wb_stage.size() < 2) {
        PipelineInst* inst = mem_stage.front();

        // Maintain in-order leaving MEM
        if (inst->mem_cycles_left > 0) {
            break;
        }

        mem_stage.pop_front();
        wb_stage.push_back(inst);

        // Update dependency map 
        int type = inst->trace_inst->inst_type;

        if (type == LOAD){
            MarkDependenceSatisfied(inst);
            used_load_mem_port = false;
        }
        else if (type == STORE){
            MarkDependenceSatisfied(inst);
            used_store_mem_port = false;
        }
    }
}

void Simulation::WritebackResults() {
    // Retire all instructions currently in WB
    while (!wb_stage.empty()) {
        PipelineInst* inst = wb_stage.front();
        wb_stage.pop_front();

        retired_count++;

        // Update simulation stats for completed instruction
        switch (inst->trace_inst->inst_type) {
            case INTEGER: cumulative_integer_inst++; break;
            case FLOATING_POINT: cumulative_fp_inst++; break;
            case BRANCH: cumulative_branch_inst++; break;
            case LOAD: cumulative_load_inst++; break;
            case STORE: cumulative_store_inst++; break;
            default: break;
        }


    }
}


//-----------------Simulation Loop--------------------------------
void Simulation::RunSimulation() {

    while (retired_count < inst_count || !PipelineEmpty()) {

            // WB -> MEM -> EX -> ID -> IF
            // Pipeline in reverse order as to not get stalled if all stages are full
            WritebackResults();
            Memoryaccess();
            InstructionIssueAndExecute();
            DecodeAndRead();
            FetchInstruction();
                
            // PrintInstructionWindow(); //For debug
            simulation_clock++;

        // Branch fetch only in the next cycle after EX completion
        if (resume_fetch_next_cycle) {
            fetch_stalled = false;
            resume_fetch_next_cycle = false;
        }
    }
}
