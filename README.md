# Spring 2026 CMPT 305 Processor Simulator Project

A high-performance C++ CPU cycle simulator developed to model pipeline depths, instruction throughput, and execution trends under heavy workloads.

## Key Features

- Simulation of a CPU. Runs heavy work-load instructions given large trace files and outputs statistics of speed and percentages of variations.
- Follows the pipeline of a basic processor structure.

## System librares required

- A x86-64 computer system under linux
- gcc version 13.3

## To run the system

1. Direct path of project folder in linux terminal.
```bash
	../proj
```

2. Type in linux terminal to compile code:
```bash
	make
```

3. Type in linux terminal with arguments to run:
```bash
	./proj srv_0 100 150000 4
```
Where srv_0 is the filename of a given trace of instructions to run, 100 (int) is the first line of instructions to run, 150000 (int) is the number of instructions to run and 4 is the pipeline depth (from 1-4). Any of such arguments can be replaced to test different metrics.