# Spring 2026 CMPT 305 Processor Simulator Project

A high-performance C++ CPU cycle simulator developed to model pipeline depths, instruction throughput, and execution trends under heavy workloads.

## Key Features

- Simulation of a CPU. Runs heavy work-load instructions given large files and outputs statistics of speed and memory usage.
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
	./proj [filename (instructions to run)] [line of first instruction (int)] [total instructions to run (int)] [pipeline depth (int)]
```