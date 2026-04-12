#include "Simulation.h"
#include "instType.h"


//-----------------Simulation Delays and Execution Times--------------------
int Simulation::GetEXLatency(int inst_type) const {
    // D=2 and D=4: FP spends 2 cycles in EX
    if ((depth_config == 2 || depth_config == 4) && inst_type == 2) {
        return 2;
    }
    return 1;
}

int Simulation::GetMEMLatency(int inst_type) const {
    // D=3 and D=4: Loads spend 3 cycles in MEM
    if ((depth_config == 3 || depth_config == 4) && inst_type == 4) {
        return 3;
    }
    return 1;
}

double Simulation::GetFrequencyGHz() const {
    switch (depth_config) {
        case 1: return 1.0;
        case 2: return 1.2;
        case 3: return 1.7;
        case 4: return 1.8;
        default: return 1.0;
    }
}

double Simulation::GetExecutionTimeMs() const {
    // time in ms = cycles 
    return (double)simulation_clock / (GetFrequencyGHz() * 1000000.0);
}


//-----------------Simulation Helpers--------------------------------
bool Simulation::PipelineEmpty() const {
    return if_stage.empty() && id_stage.empty() && ex_stage.empty() &&
           mem_stage.empty() && wb_stage.empty();
}

void Simulation::MarkDependenceSatisfied(PipelineInst* inst) {
    if (inst == nullptr || inst->result_ready) {
        return;
    }

    inst->result_ready = true;
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

    PipelineInst* inst = new PipelineInst;
    all_insts.push_back(inst);
    inst->trace_inst = src;
    inst->seq_num = next_seq_num++;
    inst->ex_cycles_left = GetEXLatency(src->inst_type);
    inst->mem_cycles_left = GetMEMLatency(src->inst_type);
    inst->unresolved_deps = 0;
    inst->result_ready = false;
    inst->entered_ex = false;
    inst->entered_mem = false;

    // dependency is on the last dynamic instance of that PC seen so far.
    for (const std::string& dep_pc : src->dependences) {
        auto it = last_dynamic_pc.find(dep_pc);
        if (it != last_dynamic_pc.end() && it->second != nullptr) {
            PipelineInst* producer = it->second;
            if (!producer->result_ready) {
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
        ElementQueueNode* src = ElementQ->GetElementAtIndex((uint64_t)fetched_count);
        if (src == nullptr) {
            return;
        }

        PipelineInst* inst = BuildFetchedInstruction(src);
        if_stage.push_back(inst);
        fetched_count++;

        // once a branch is fetched, stop fetching following instructions
        if (src->inst_type == 3) {
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
    int moved_to_mem = 0;
    bool used_load_mem_port = false;
    bool used_store_mem_port = false;

    while (!ex_stage.empty() && moved_to_mem < 2) {
        PipelineInst* inst = ex_stage.front();

        // in-order leaving EX
        if (inst->ex_cycles_left > 0) {
            break;
        }

        int type = inst->trace_inst->inst_type;

        if (type == 4) {
            if (used_load_mem_port) break;
            used_load_mem_port = true;
        }
        if (type == 5) {
            if (used_store_mem_port) break;
            used_store_mem_port = true;
        }

        ex_stage.pop_front();
        inst->entered_mem = true;
        mem_stage.push_back(inst);
        moved_to_mem++;

        if (type == INTEGER || type == FLOATING_POINT || type == BRANCH) {
            MarkDependenceSatisfied(inst);
        }

        // Branch resolves after EX completes.
        if (type == BRANCH) {
            resume_fetch_next_cycle = true;
        }
    }

    // Issue from ID to EX
    int issued_this_cycle = 0;
    bool used_int_unit = false;
    bool used_fp_unit = false;
    bool used_branch_unit = false;

    while (!id_stage.empty() && issued_this_cycle < 2) {
        PipelineInst* inst = id_stage.front();
        int type = inst->trace_inst->inst_type;

        // Must wait for data dependences
        if (inst->unresolved_deps > 0) {
            break; // later instructions in ID cannot pass this one
        }

        // Structural hazards for EX 
        if (type == INTEGER && used_int_unit) break;
        if (type == FLOATING_POINT && used_fp_unit) break;
        if (type == BRANCH && used_branch_unit) break;

        id_stage.pop_front();
        inst->entered_ex = true;
        ex_stage.push_back(inst);
        issued_this_cycle++;

        if (type == INTEGER) used_int_unit = true;
        if (type == FLOATING_POINT) used_fp_unit = true;
        if (type == BRANCH) used_branch_unit = true;
    }
}

void Simulation::Memoryaccess() {
    // Advance instructions already in MEM
    for (PipelineInst* inst : mem_stage) {
        if (inst->mem_cycles_left > 0) {
            inst->mem_cycles_left--;
        }
    }

    // Move completed MEM instructions to WB in-order
    int moved_to_wb = 0;
    while (!mem_stage.empty() && moved_to_wb < 2) {
        PipelineInst* inst = mem_stage.front();

        // Maintain in-order leaving MEM
        if (inst->mem_cycles_left > 0) {
            break;
        }

        mem_stage.pop_front();
        wb_stage.push_back(inst);
        moved_to_wb++;

        int type = inst->trace_inst->inst_type;
        if (type == LOAD || type == STORE) {
            MarkDependenceSatisfied(inst);
        }
    }
}

void Simulation::WritebackResults() {
    // Retire all instructions currently in WB
    while (!wb_stage.empty()) {
        PipelineInst* inst = wb_stage.front();
        wb_stage.pop_front();

        retired_count++;
        cumulative_execution = simulation_clock;

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
        // if (simulation_clock % 100000 == 0) {
            //     printf("Cycle: %d | Retired: %d\n", simulation_clock, retired_count);
            // }
            // WB -> MEM -> EX -> ID -> IF
            WritebackResults();
            Memoryaccess();
            InstructionIssueAndExecute();
            DecodeAndRead();
            FetchInstruction();
            
            simulation_clock++;

        // Branch fetch only in the next cycle after EX completion
        if (resume_fetch_next_cycle) {
            fetch_stalled = false;
            resume_fetch_next_cycle = false;
        }
    }
}
