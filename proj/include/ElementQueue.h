#include <cstdint>
#include <vector>
#include <string>

#ifndef ELEMENT_QUEUE_H_
#define ELEMENT_QUEUE_H_

//-----------------Instruction Node------------------------------------
/**
    Represents a single instruction (node) from the trace file
 */
struct ElementQueueNode {
    double arrival_time;                    // Instruction arrival time - depend on when instruction is fetched
    double service_time;                    // Instruction service time (deterministic)  - depend on depth and instruction type
    std::string program_counter;            // Instruction program counter (instruction address)
    int inst_type;                          // Instruction type (1:int_alu, 2:fp, 3:branch, 4:load, 5:store)
    std::vector<std::string> dependences;   // List of inst PCs that current is dependent on
};


//-----------------Node Queue------------------------------------
/**
    Queue of instructions that are to be simulated
 */
class ElementQueue {
	public:
        ElementQueue(char* filename, int start_inst, int inst_count);
        ~ElementQueue();
        ElementQueueNode* GetCurrentElement();
        ElementQueueNode* GetElementAtIndex(uint64_t index);
        ElementQueueNode* AdvanceToNextElement();

        void PrintElementQueue(); // for debugging purposes

    private:
        void InitializeQueue(char* filename, int start_inst, int inst_count);
        uint64_t size;                              // total size of element queue
        uint64_t current;                           // Point to the current node being processed for arrival event
        std::vector<ElementQueueNode> ElementArray; // Array containing all instructions to run, loaded at the beginning of simulation
};


#endif