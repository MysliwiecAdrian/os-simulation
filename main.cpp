/*
    Name: Adrian Mysliwiec
    Course: CSCI340 Operating System
    Description: Test driver
    File: main.cpp
*/

//command to run: g++ -std=c++17 *.cpp -o runme

#include "SimOS.h"
#include <iostream>

int main()
{
    SimOS sim(1, 1000000, 1000);

    MemoryUsage memUsage = sim.GetMemory();
    std::cout << "Memory Usage: " << memUsage[1].PID << std::endl;;

    sim.NewProcess();
    sim.NewProcess();
    sim.NewProcess();
    sim.NewProcess();

    /*
        Test Case 1: Checks if the 4 new processes are added correctly
        Output: 1 2 3 4
    */ 
    std::deque<int> readyQueue = sim.GetReadyQueue();
    std::cout << sim.GetCPU() << " ";
    for (int i = 0; i < readyQueue.size(); i++)
        std::cout << readyQueue[i] << " ";
    std::cout << std::endl;

    /*
        Test Case 2: Checks if timer interrupt updates the CPU and ready queue correctly
        Output: 4 1 2 3
    */
    sim.TimerInterrupt();
    sim.TimerInterrupt();
    sim.TimerInterrupt();
    readyQueue = sim.GetReadyQueue();
    std::cout << sim.GetCPU() << " ";
    for (int i = 0; i < readyQueue.size(); i++)
        std::cout << readyQueue[i] << " ";
    std::cout << std::endl;

    /*
        Test Case 3: Checks if SimFork() creates a new process correctly
        Output: 4 1 2 3 5
    */
    sim.SimFork();
    readyQueue = sim.GetReadyQueue();
    std::cout << sim.GetCPU() << " ";
    for (int i = 0; i < readyQueue.size(); i++)
        std::cout << readyQueue[i] << " ";
    std::cout << std::endl;

}