#include <fstream>
#include <iostream>
#include <string>
#include <bits/stdc++.h>

#include "ElementQueue.h"


ElementQueue::ElementQueue(char* filename, int start_inst, int inst_count){
    size = inst_count;
    current = 0; // current element is the first element in the element array
    InitializeQueue(filename, start_inst, inst_count);
    if ((int)ElementArray.size() != inst_count) {
        printf("Failed to load all required instructions %d %d.\n", (int)ElementArray.size(), inst_count);
        exit(1);
    }
}

ElementQueue::~ElementQueue() {}

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

void ElementQueue::PrintElementQueue() {
    // For debug purposes, not recommended for large instruction counts
    for (size_t i = 0; i < ElementArray.size(); i++){
        printf("a_t: %f, s_t %f, PC: %s, T: %d, D: ", ElementArray[i].arrival_time, ElementArray[i].service_time, ElementArray[i].program_counter.c_str(), ElementArray[i].inst_type);
        for (size_t j = 0; j < ElementArray[i].dependences.size(); j++){
            printf("%s ", ElementArray[i].dependences[j].c_str());
        }
        printf("\n");
    }
}

void ElementQueue::InitializeQueue(char* filename, int start_inst, int inst_count) {

    std::ifstream file(filename);
    std::string str;
    std::string file_contents;
    
    char delim = ',';

    int i = 1;
    while (std::getline(file, str))
    {
        if (i >= start_inst){
            std::vector<std::string> instructions;
            std::stringstream ss (str);
            std::string component;

            ElementQueueNode qnode;
            qnode.arrival_time = -1;
            qnode.service_time = 1; // depend on D and inst type

            getline (ss, component, delim);
            qnode.program_counter = component;

            getline (ss, component, delim);
            qnode.inst_type = atoi(component.c_str());

            while (getline (ss, component, delim)) {
                qnode.dependences.push_back (component);
            }

            ElementArray.push_back(qnode);

        }
        if (i >= start_inst + inst_count - 1){
            break;
        }
        i++;
    }  
}
