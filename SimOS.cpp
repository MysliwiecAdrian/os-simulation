/*
    Name: Adrian Mysliwiec
    Course: CSCI340 Operating System
    Description: Executes the simulation of an operating system
    File: SimOS.cpp
*/

#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize){
    this->numberOfDisks = numberOfDisks;
    for (int i = 0; i < numberOfDisks; i++)
        disks.push_back(Disk());

    numOfRAM = amountOfRAM;
    this->pageSize = pageSize;
}

void SimOS::NewProcess(){
    Process newProcess(PID);
    if (runningCPU == NO_PROCESS){
        //enter ready queue
        readyQueue.push_back(newProcess.getPID());
        newProcess.setState(READY);

        //enter CPU
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        newProcess.setState(RUNNING);
    }
    else{
        //enter ready queue and wait
        readyQueue.push_back(newProcess.getPID());
        newProcess.setState(READY);
    }

    //store process in OS
    processes[PID] = newProcess;
    PID++;
}

void SimOS::SimFork(){
    //check if process is running
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");

    //create new process
    Process newProcess(PID, runningCPU);
    processes[PID] = newProcess;
    
    //enter ready queue
    readyQueue.push_back(newProcess.getPID());
    processes[PID].setState(READY);

    //add child to parent
    processes[runningCPU].addChild(PID);
    
    PID++;
}

void SimOS::SimExit(){
    //check if process is running
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");

    if (processes[runningCPU].getParent() == 0){ //running process is parent
        //terminates all children
        cascadeTerminate(runningCPU); 

        //removes process from ready queue
    

        //removes process from I/O queues
        

        processes[runningCPU].setState(TERMINATED);
        runningCPU = NO_PROCESS;

        //moves next process in ready queue to CPU
        if (!readyQueue.empty()){
            runningCPU = readyQueue.front();
            readyQueue.pop_front();
            processes[runningCPU].setState(RUNNING);
        }
    }
        
}

void SimOS::SimWait(){
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");
    processes[runningCPU].setState(WAITING);
}

void SimOS::TimerInterrupt(){
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");

    processes[runningCPU].setState(WAITING);
    readyQueue.push_back(runningCPU);
    runningCPU = readyQueue.front();
    processes[runningCPU].setState(RUNNING);
    readyQueue.pop_front();
}

void SimOS::DiskReadRequest(int diskNumber, std::string fileName){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");
    processes[runningCPU].setState(WAITING);
    disks[diskNumber].addRequest({runningCPU, fileName});
}

void SimOS::DiskJobCompleted(int diskNumber){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");

    int completedJob = disks[diskNumber].completeRequest();
    if (completedJob == NO_PROCESS) {
        throw std::logic_error("No process is waiting for disk job to complete");
    } 
    else if (runningCPU == NO_PROCESS){
        //enter ready queue
        readyQueue.push_back(completedJob);
        processes[completedJob].setState(READY);

        //enter CPU
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        processes[completedJob].setState(RUNNING);
    }
    else {
        readyQueue.push_back(completedJob);
        processes[completedJob].setState(READY);
    }

}

void SimOS::AccessMemoryAddress(unsigned long long address){

}

int SimOS::GetCPU(){
    return runningCPU;
}

std::deque<int> SimOS::GetReadyQueue(){
    return readyQueue;
}

MemoryUsage SimOS::GetMemory(){
    return memory;
}

FileReadRequest SimOS::GetDisk(int diskNumber){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");

    return disks[diskNumber].getRequest();
}

std::deque<FileReadRequest> SimOS::GetDiskQueue(int diskNumber){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");

    return disks[diskNumber].getDiskQueue();
}

//helper functions
void SimOS::cascadeTerminate(int PID){
    for (int child : processes[PID].getChildren()){
        //removes from ready queue
        for (int i = 0; i < readyQueue.size(); i++){
            if (readyQueue[i] == child){
                readyQueue.erase(readyQueue.begin() + i);
                break;
            }
        }

        //removes from I/O queue
        
        processes[child].setState(TERMINATED);
        cascadeTerminate(child);
    }
}

//tester functions
Process SimOS::getProcess(int PID){
    return processes[PID];
}