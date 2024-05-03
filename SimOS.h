/*
    Name: Adrian Mysliwiec
    Course: CSCI340 Operating System
    Description: Creates and executes the interface simulation of an operating system
    File: SimOS.h
*/

#ifndef SIMOS_H
#define SIMOS_H

#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <stdexcept>
#include "Process.h"
#include "Disk.h"

struct MemoryItem
{
    unsigned long long pageNumber;
    unsigned long long frameNumber;
    int PID; // PID of the process using this frame of memory
};
 
using MemoryUsage = std::vector<MemoryItem>;
constexpr int NO_PROCESS{ 0 };

class SimOS
{   
    public:
        SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize);
        void NewProcess();
        void SimFork();
        void SimExit();
        void SimWait();
        void TimerInterrupt();
        void DiskReadRequest(int diskNumber, std::string fileName);
        void DiskJobCompleted(int diskNumber);
        void AccessMemoryAddress(unsigned long long address);
        int GetCPU();
        std::deque<int> GetReadyQueue();
        MemoryUsage GetMemory();
        FileReadRequest GetDisk(int diskNumber);
        std::deque<FileReadRequest> GetDiskQueue(int diskNumber);

        //helper functions
        void cascadeTerminate(int PID);

        //tester functions
        Process getProcess(int PID);

    private:
        int numberOfDisks = 0;
        unsigned long long numOfRAM;
        unsigned int pageSize;
        int runningCPU = NO_PROCESS;
        int PID = 1;

        std::vector<Disk> disks;
        MemoryUsage memory;
        std::deque<int> readyQueue;
        std::map<int, Process> processes;
};

#endif