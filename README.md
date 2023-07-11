```
   ____ _____    ___  ____    ____ ___ __  __ _   _ _        _  _____ ___ ___  _   _ 
  / ___|___  |  / _ \/ ___|  / ___|_ _|  \/  | | | | |      / \|_   _|_ _/ _ \| \ | |
 | |  _   / /  | | | \___ \  \___ \| || |\/| | | | | |     / _ \ | |  | | | | |  \| |
 | |_| | / /   | |_| |___) |  ___) | || |  | | |_| | |___ / ___ \| |  | | |_| | |\  |
  \____|/_/     \___/|____/  |____/___|_|  |_|\___/|_____/_/   \_\_| |___\___/|_| \_|

```
## Objectives
The objectives of this simulation are to demonstrate the process and memory management functionalities of an operating system. The simulation focuses on the following aspects: 

1. Process Control Block (PCB) management: Creation, scheduling, termination, blocking, and waking up of processes.
2. Memory management: Allocation and deallocation of memory partitions using the First Fit algorithm.
3. Interaction with the user through a command-line interface.

## Overview
The simulation utilizes a multi-threaded approach to simulate the behavior of an operating system. The main thread handles user input and interacts with the operating system functions, while a separate thread runs the operating system, managing processes and memory.\\

The simulation includes the following major components:
1. Process Control Block (PCB): Represents a process and contains information such as process ID, arrival time, required time, used time, state, size, and start address in memory. The PCBs are organized as a linked list.
2. Memory Management: Manages memory partitions using a Free Memory Partition Table (FMPT) and supports memory allocation and deallocation using the First Fit algorithm.
3. Operating System Functions: Includes functions for initializing the PCBs and memory, starting the operating system, and implementing process scheduling, termination, blocking, and waking up.
4. User Interface: Provides a command-line interface for the user to interact with the simulation, create PCBs, and view the status of PCBs and memory.

## How to run
Compile the [Code](https://github.com/hungphongtrn/OS_SYSTEM_PRJ/blob/main/G7_OS_SIMULATION.cpp) and use the console menu to navigate!


                                                                                                                
