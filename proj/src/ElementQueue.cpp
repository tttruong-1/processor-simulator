#include <fstream>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

#include "ElementQueue.h"

//-----------------Queue Setup------------------------------------
ElementQueue::ElementQueue(char* filename, int start_inst, int inst_count){
    size = inst_count;
    current = 0; // current element is the first element in the element array
    InitializeQueue(filename, start_inst, inst_count);
    if ((int)ElementArray.size() != inst_count) { // out of bounds
        printf("Failed to load all required instructions %d %d.\n", (int)ElementArray.size(), inst_count);
        exit(1);
    }
}

ElementQueue::~ElementQueue() {}

//-----------------Queue Functions------------------------------------
ElementQueueNode* ElementQueue::GetCurrentElement() {
    return &ElementArray[current];
}

ElementQueueNode* ElementQueue::GetElementAtIndex(uint64_t index) {
    if (index < size)
        return &ElementArray[index];
    else	
        return nullptr;
}

ElementQueueNode* ElementQueue::AdvanceToNextElement() {
    if (current < (size - 1)) {
        current++;
        return &ElementArray[current];
    } 
    else
        return nullptr; 
}

/**
    For debug purposes, not recommended for large instruction counts
    Prints the instructions on each line
 */
void ElementQueue::PrintElementQueue() {
    for (size_t i = 0; i < ElementArray.size(); i++){
        printf("a_t: %f, s_t %f, PC: %s, T: %d, D: ", ElementArray[i].arrival_time, ElementArray[i].service_time, ElementArray[i].program_counter.c_str(), ElementArray[i].inst_type);
        for (size_t j = 0; j < ElementArray[i].dependences.size(); j++){
            printf("%s ", ElementArray[i].dependences[j].c_str());
        }
        printf("\n");
    }
}


//-----------------Queue Intialization------------------------------------
/**
    Reads the file, creates instruction nodes and create node queue

    Instruction Line:
    "instruction program counter (address)", "instruction type", "dependency (if any)"

    Examples:
    ffffb7e17cb0,1
    ffff000008082840,1
    ffff000008082844,5,ffff000008082840
    ffff00000808284c,5,ffff000008082840,ffffb7e17cb4,ffffb7e17cb8
 */
void ElementQueue::InitializeQueue(char* filename, int start_inst, int inst_count) {

    std::ifstream file(filename);
    std::string str;
    std::string file_contents;
    
    char delim = ','; // comma separated

    int i = 1; // line number in file (counter)
    while (std::getline(file, str))
    {
        if (i >= start_inst){ // skip lines until start at line start_inst
            std::vector<std::string> instructions;
            std::stringstream ss (str);
            std::string component;

            ElementQueueNode qnode;
            qnode.arrival_time = -1; // to be determined upon instruction fetched
            qnode.service_time = 1; // depend on D and inst type

            // parse line
            getline (ss, component, delim);
            qnode.program_counter = component; // instruction program counter

            getline (ss, component, delim);
            qnode.inst_type = static_cast<instType>(atoi(component.c_str())); // instruction type (1-5)

            while (getline (ss, component, delim)) {
                qnode.dependences.push_back (component); // dependences
            }

            // add the parse instruction to queue
            ElementArray.push_back(qnode);

        }
        if (i >= start_inst + inst_count - 1){ // number of instructions to run has been met
            break;
        }
        i++;
    }  
}
